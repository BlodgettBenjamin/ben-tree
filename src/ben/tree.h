#pragma once

#include "type.h"
#include "memory.h"
#include "assert.h"

namespace btl
{
	template <class StartArgs, class... Args>
	class tree
	{
	public:
		using pack_t = pack<StartArgs, Args...>;
	public:
		static constexpr u64 const count_t = pack_t::count;
		static constexpr u64 const leaf_t = pack_t::count - 1;
		static constexpr u64 const base_t = 0;
		// x----------------------------------------------------------------------------------------------x
		// |   - constexpr functions have turned out to suck really bad                                   |
		// |   - should make these non constexpr and have them as "getters" instead                       |
		// x----------------------------------------------------------------------------------------------x
		//
		template <class LayerType> constexpr u64 index() const;
		template <class LayerType> u64 size() const;
		template <class LayerType> LayerType* ptr();
		template <class LayerType> u64 value_index(const LayerType& value) const;

		tree();
		// x----------------------------------------------------------------------------------------------x
		// |   - increments layer size                                                                    |
		// |   - adds specified data to corresponding layer allocation                                    |
		// |   - adds conceptual branches of default count 0 to current layer (if it isnt a leaf layer)   |
		// |   - adds new data count to the parent's branches at index "ParentBranch"                     |
		// |   - stores index of parent branch                                                            |
		// x----------------------------------------------------------------------------------------------x
		//
		template <class LayerType>
		void add(u64 parent_index, const LayerType* data_ptr, u64 count = 1);
		// x----------------------------------------------------------------------------------------------x
		// |   - used to add with default parent_index of zero                                            |
		// x----------------------------------------------------------------------------------------------x
		//
		template <class LayerType>
		void add(const LayerType* data_ptr, u64 count = 1);
		// x----------------------------------------------------------------------------------------------x
		// |   - same as regular add but a const reference to the specified parent can be specified       |
		// |     instead of explicitly specifying the parent branch index as a template parameter         |
		// x----------------------------------------------------------------------------------------------x
		//
		template <class ParentLayerType, class LayerType>
		void add_child(const ParentLayerType& parent, const LayerType* data_ptr, u64 count = 1);
		// x----------------------------------------------------------------------------------------------x
		// |   - takes const reference of an actual tree value and returns the associated parent          |
		// |     reference using internal parent indices data                                             |
		// |   - should not be called for child type corresponding to layer 0                             |
		// x----------------------------------------------------------------------------------------------x
		//
		template <class ParentLayerType, class LayerType>
		const ParentLayerType& get_parent(const LayerType& tree_val) const;
		void print_conceptual() const;
	private:
		u64   layer_sizes[count_t] = { 0 };
		void* layer_data[count_t] = { 0 };
		// x----------------------------------------------------------------------------------------------x
		// |   - layer_branches[leaf_t] must always be nullptr                                            |
		// |   - layer_branches[t] stores a conceptual "branch" which is just the number of children in   |
		// |     the next layer who consider layer_branch[t] their parent                                 |
		// x----------------------------------------------------------------------------------------------x
		//
		u64*  layer_branches[count_t] = { 0 };
		// x----------------------------------------------------------------------------------------------x
		// |   - layer_parent_index[0] must always be nullptr                                             |
		// |   - layer_parent_index[t] stores the associated index of its parent in layer t - 1           |
		// x----------------------------------------------------------------------------------------------x
		//
		u64*  layer_parent_indices[count_t] = { 0 };
	};

	namespace layer
	{

		// x----------------------------------------------------------------------------------------------x
		// |   - btl::layer::iterator iterates the flat allocations of btl::tree                          |
		// |   - use of btl::make_iterable() is necessary for range based for loops                       |
		// x----------------------------------------------------------------------------------------------x
		//
		template <class tree_t, typename LayerType>
		class iterator
		{
		public:
			iterator(LayerType* layer_ptr)
				: ptr(layer_ptr)
			{};
			iterator& operator++() { ptr++; return *this; }
			iterator operator++(i32) { iterator it = *this; ++(*this); return it; }
			iterator& operator--() { ptr--; return *this; }
			iterator operator--(i32) { iterator it = *this; --(*this); return it; }
			LayerType& operator[](i32 index) { return *(ptr + index); }
			LayerType* operator->() { return ptr; }
			LayerType& operator*() { return *ptr; }
			bool operator==(const iterator& other) const { return ptr == other.ptr; }
			bool operator!=(const iterator& other) const { return ptr != other.ptr; }
		private:
			LayerType* ptr = nullptr;
		};

		// x----------------------------------------------------------------------------------------------x
		// |   - btl::layer::iterable is necessary for btl::make_iterable to work nicely with btl::tree   |
		// |   - btl::tree does not implement an iterator but this will likely change in the future       |
		// x----------------------------------------------------------------------------------------------x
		// |   - don't try to declare a type of btl::layer::iterable                                      |
		// x----------------------------------------------------------------------------------------------x
		//
		template <typename LayerType, class tree_t>
		class iterable
		{
		public:
			iterable(LayerType* data_ptr, LayerType* data_ptr_end)
				: p0(data_ptr), p1(data_ptr_end)
			{};

			layer::iterator<tree_t, LayerType> begin() { return layer::iterator<tree_t, LayerType>(p0); }
			layer::iterator<tree_t, LayerType> end() { return layer::iterator<tree_t, LayerType>(p1); }
		private:
			LayerType* const p0;
			LayerType* const p1;
		};
	}


	// x----------------------------------------------------------------------------------------------x
	// |   -  btl::make_iterable is currently used for making btl::tree work with range based for     |
	// |      loops                                                                                   |
	// |   -  it could be implemented for types other than btl::tree in the future but                |
	// |      disambiguation will make its use a bit more cumbersome                                  |
	// x----------------------------------------------------------------------------------------------x
	// |   EXAMPLE USAGE :                                                                            |
	// x----------------------------------------------------------------------------------------------x
	// |         for (const auto& vector : btl::make_iterable<vec3>(&tree)                            |
	// |                                    or                                                        |
	// |         for (const vec3& vector : btl::make_iterable<vec3>(&tree)                            |
	// |                                                                                              |
	// |                                   never                                                      |
	// |         for (iterable<vec3, tree<>> vector : btl::make_iterable<vec3>(&tree)                 |
	// x----------------------------------------------------------------------------------------------x
	//
	template <typename LayerType, class tree_t>
	layer::iterable<LayerType, tree_t> make_iterable(tree_t* tree)
	{
		const auto p0 = tree->ptr<LayerType>();
		const auto p1 = tree->ptr<LayerType>() + tree->size<LayerType>();
		return layer::iterable<LayerType, tree_t>(p0, p1);
	}

	template <class StartArgs, class... Args>
	template <class LayerType> constexpr u64 tree<StartArgs, Args...>
	::index() const
	{
		/* asserts */ {
		// tree must contain LayerType                                                         0
		static_assert(pack_t::alias::contains<LayerType>::value);//                            0
		}
		return pack_t::alias::index<LayerType>::value;
	}

	template <class StartArgs, class... Args>
	template <class LayerType> u64 tree<StartArgs, Args...>
	::size() const
	{
		/* asserts */ {
		// tree must contain LayerType                                                         0
		static_assert(pack_t::alias::contains<LayerType>::value,                              "0");
		}
		return layer_sizes[index<LayerType>()];
	}

	template <class StartArgs, class... Args>
	template <class LayerType> LayerType* tree<StartArgs, Args...>
	::ptr()
	{
		/* asserts */ {
		// tree must contain LayerType                                                         0
		static_assert(pack_t::alias::contains<LayerType>::value,                              "0");
		}
		return reinterpret_cast<LayerType*>(layer_data[index<LayerType>()]);
	}

	template <class StartArgs, class... Args>
	template <class LayerType> u64 tree<StartArgs, Args...>
	::value_index(const LayerType& value) const
	{
		const constexpr u64 t = pack_t::alias::index<LayerType>::value;
		const LayerType* p0 = reinterpret_cast<LayerType*>(layer_data[t]);
		const LayerType* p1 = &value;                                                                                          assert(p0 <= p1);
		return btl::pointer_offset(p0, p1);
	}

	template <class StartArgs, class... Args>
	tree<StartArgs, Args...>
	::tree()
	{
		/* asserts */ {
		// tree must contain all parameters which it contains                                  0   
		// all pack parameters must be reflexive                                               1   
		// tree cannot contain duplicate types                                                 2   
		// tree layers cannot be comprised of pointers                                         3   
		// tree layers cannot be comprised of references                                       4   
		// all pack parameters must be trivially copyable                                      5   
		static_assert(pack_t::containment == count_t,                                         "0"); 
		static_assert(pack_t::reflexiveness == count_t,                                       "1"); 
		static_assert(pack_t::is_set,                                                         "2");
		static_assert(pack_t::pointedness == 0,                                               "3"); 
		static_assert(pack_t::referentiality == 0,                                            "4"); 
		static_assert(pack_t::triviality == count_t,                                          "5"); 
		}
		u64* layer_parent_indices[count_t] = { 0 };
		ben::printf("sizeof(layer_sizes): %u\n", sizeof(layer_sizes));
		ben::printf("sizeof(layer_data): %u\n", sizeof(layer_data));
		ben::printf("sizeof(layer_branches): %u\n", sizeof(layer_branches));
		ben::printf("sizeof(layer_parent_indices): %u\n", sizeof(layer_parent_indices));
		assert(ben::bytes_same(this) >= sizeof(this));
		assert(layer_sizes[0] == 0);
	}

	template <class StartArgs, class... Args>
	template <class LayerType> void tree<StartArgs, Args...>
	::add(u64 parent_index, const LayerType* data_ptr, u64 count)
	{
		/* asserts */ {
		// tree must contain specified type                                                    0
		// if LayerType equals base_t then parent_index must equal 0                           1
		static_assert(pack_t::alias::contains<LayerType>::value,                              "0");
		BEN_ASSERT(pack_t::alias::index<LayerType>::value != base_t || parent_index == 0,     "1");
		}
		const constexpr u64 t = pack_t::alias::index<LayerType>::value;

		const u64 old_count = layer_sizes[t];
		layer_sizes[t] += count;
		const u64 new_count = layer_sizes[t];

		auto typed_data_ptr = reinterpret_cast<LayerType*>(layer_data[t]);
		layer_data[t] = btl::append_allocation(typed_data_ptr, data_ptr, old_count, new_count);

		if (t > base_t)
		{
#			pragma warning( push )
#			pragma warning( disable : 6201 )
			btl::append_set_allocation(layer_parent_indices[t], parent_index, old_count, new_count);                           assert(layer_branches[t - 1] != nullptr);
			const constexpr u64 parent_t = t - 1;
			layer_branches[parent_t][parent_index] += count;
#			pragma warning( pop )
		}
		if (t < leaf_t)
		{
			btl::append_allocation0(layer_branches[t], old_count, new_count);
		}
	}

	template <class StartArgs, class... Args>
	template <class LayerType> void tree<StartArgs, Args...>
	::add(const LayerType* data_ptr, u64 count)
	{
		add(0, data_ptr, count);
	}

	template <class StartArgs, class... Args>
	template <class ParentLayerType, class LayerType> void tree<StartArgs, Args...>
	::add_child(const ParentLayerType& parent, const LayerType* data_ptr, u64 count)
	{
		const constexpr u64 t = pack_t::alias::index<LayerType>::value;
		const constexpr u64 branch = value_index(parent);                                                                      assert(branch < layer_sizes[base_t]);

		add<branch>(data_ptr, count);
	}


	template <class StartArgs, class... Args>
	template <class ParentLayerType, class LayerType> const ParentLayerType& tree<StartArgs, Args...>
	::get_parent(const LayerType& tree_val) const
	{
		/* asserts */ {
		// do not attempt to get the equal type parent of tree_val type                        0 
		// do not this tree's parameter pack into get_parent                                   1
		// do not pass a tree of this tree type into get_parent                                2
		// tree must contain ParentLayerType                                                   3
		// tree must contain LayerType                                                         4
		// specified ParentLayerType must be a parent type of tree_val                         5
		const constexpr auto parent_index = pack_t::alias::index<ParentLayerType>::value;
		const constexpr auto child_index = pack_t::alias::index<LayerType>::value;
		static_assert(std::is_same<LayerType, ParentLayerType>::value == false,               "0");
		static_assert(std::is_same<pack_t, LayerType>::value == false,                        "1");
		static_assert(std::is_same<LayerType, tree<StartArgs, Args...>>::value == false,      "2");
		static_assert(pack_t::alias::contains<ParentLayerType>::value,                        "3");
		static_assert(pack_t::alias::contains<LayerType>::value,                              "4");
		static_assert(parent_index < child_index,                                             "5");
		}
		const constexpr u64 this_layer = pack_t::alias::index<LayerType>::value;
		const constexpr u64 parent_layer = pack_t::alias::index<ParentLayerType>::value;

		u64 current_index = value_index(tree_val);                                                                             assert(current_index < layer_sizes[this_layer]);
		                                                                                                                       assert(layer_parent_indices[this_layer] != nullptr);
		u64* parent_indices_ptr = layer_parent_indices[this_layer];		                                                       assert(this_layer != 0);
		u64 parent_index = *(parent_indices_ptr + current_index);                                                              assert(parent_index < layer_sizes[parent_layer]);


		auto parent_ptr = reinterpret_cast<ParentLayerType*>(layer_data[parent_layer]);                                        assert(parent_ptr != nullptr);
		parent_ptr += parent_index;

		return *parent_ptr;
	}


	template <class StartArgs, class... Args>
	void tree<StartArgs, Args...>::print_conceptual() const
	{
		for (u64 t = base_t; t < leaf_t; t++)
		{
			ben::printf("layer %u:\n", t);
			if (layer_sizes[t] > 0)
			{
				ben::printf("{");
				for (u64 i = 0; i < layer_sizes[t]; i++)
				{
					ben::str120 fmt = i == layer_sizes[t] - 1 ? "%llu" : "%llu, ";
					u64 branch_value = *(layer_branches[t] + i);
					ben::printf(fmt, branch_value);
				}
				ben::printf("}\n");
			}
		}
	}
}