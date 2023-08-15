#pragma once

#include "type.h"
#include "memory.h"

namespace btl
{
	template <class StartArgs, class... Args>
	class tree
	{
	public:
		using pack_t = pack<StartArgs, Args...>;
	public:
		static constexpr u64 const count_t = pack_t::count;
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
		// |   - the default ParentBranch value should be used for the 0th layer                          |
		// x----------------------------------------------------------------------------------------------x
		//
		template <u64 ParentBranch = U64_MAX, class LayerType> void add(const LayerType* data_ptr, u64 count = 1);
		template <class ParentLayerType, class LayerType> const ParentLayerType& get_parent(const LayerType& tree_val) const;
		void print_conceptual() const;
	private:
		u64   layer_sizes[count_t] = { 0 };
		void* layer_data[count_t] = { 0 };
		// x----------------------------------------------------------------------------------------------x
		// |   - layer_branches[count_t - 1] must always be nullptr                                       |
		// |   - layer_branches[i] stores a conceptual "branch" which is just the number of children in   |
		// |     the next layer who consider layer_branch[i] their parent                                 |
		// x----------------------------------------------------------------------------------------------x
		//
		u64*  layer_branches[count_t] = { 0 };
		// x----------------------------------------------------------------------------------------------x
		// |   - layer_parent_index[0] must always be nullptr                                             |
		// |   - layer_parent_index[i] stores the associated index of its parent in layer i - 1           |
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
		return layer::iterable<LayerType, tree_t>(tree->ptr<LayerType>(), tree->ptr<LayerType>() + tree->size<LayerType>());
	}

	template <class StartArgs, class... Args>
	template <class LayerType> constexpr u64 tree<StartArgs, Args...>::index() const
	{
		static_assert(pack_t::alias::contains<LayerType>::value, "tree does not contain LayerType");
		return pack_t::alias::index<LayerType>::value;
	}

	template <class StartArgs, class... Args>
	template <class LayerType> u64 tree<StartArgs, Args...>::size() const
	{
		static_assert(pack_t::alias::contains<LayerType>::value, "tree does not contain LayerType");
		return layer_sizes[index<LayerType>()];
	}

	template <class StartArgs, class... Args>
	template <class LayerType> LayerType* tree<StartArgs, Args...>::ptr()
	{
		static_assert(pack_t::alias::contains<LayerType>::value, "tree does not contain LayerType");
		return reinterpret_cast<LayerType*>(layer_data[index<LayerType>()]);
	}

	template <class StartArgs, class... Args>
	template <class LayerType> u64 tree<StartArgs, Args...>::value_index(const LayerType& value) const
	{
		const LayerType* p0 = reinterpret_cast<LayerType*>(layer_data[pack_t::alias::index<LayerType>::value]);
		const LayerType* p1 = &value;                                                                                          assert(p0 <= p1);
		return btl::pointer_offset(p0, p1);
	}

	template <class StartArgs, class... Args>
	tree<StartArgs, Args...>::tree()
	{
		static_assert(pack_t::reflexiveness == count_t, "all pack parameters must be reflexive");
		static_assert(pack_t::triviality == count_t, "all pack parameters must be trivially copyable");
		static_assert(pack_t::is_set, "tree cannot contain duplicate types");
		static_assert(pack_t::pointedness == 0, "tree layers cannot be comprised of pointers");
		static_assert(pack_t::containment == count_t, "tree must contain all parameters which it contains");

		u64* layer_parent_indices[count_t] = { 0 };
		ben::printf("sizeof(layer_sizes): %u\n", sizeof(layer_sizes));
		ben::printf("sizeof(layer_data): %u\n", sizeof(layer_data));
		ben::printf("sizeof(layer_branches): %u\n", sizeof(layer_branches));
		ben::printf("sizeof(layer_parent_indices): %u\n", sizeof(layer_parent_indices));
		assert(ben::bytes_same(this) >= sizeof(this));
		assert(layer_sizes[0] == 0);
	}

	template <class StartArgs, class... Args>
	template <u64 ParentBranch, class LayerType> void tree<StartArgs, Args...>::add(const LayerType* data_ptr, u64 count)
	{
		static_assert(pack_t::alias::contains<LayerType>::value, "tree does not contain specified type");
		static_assert(ParentBranch != 0 || pack_t::alias::index<LayerType>::value != 0, "ParentBranch parameter specified for layer 0 which does not have a parent layer!");

		const constexpr u64 i = pack_t::alias::index<LayerType>::value;
		layer_sizes[i] += count;
		const constexpr u64 parent_branch_index = ParentBranch == U64_MAX ? 0 : ParentBranch;
		const u64 new_type_count = layer_sizes[i];
		const u64 old_type_count = new_type_count - count;
		void*& data = layer_data[i];
		u64*& branches = layer_branches[i];
		u64*& parent_indices = layer_parent_indices[i];

		void* data_realloc_ptr = realloc(data, sizeof(LayerType) * new_type_count);                                            assert(data_realloc_ptr != nullptr);
		data = data_realloc_ptr;

		void* data_copy_dst_ptr = reinterpret_cast<LayerType*>(data) + old_type_count;
		void* data_copy_result = memcpy(data_copy_dst_ptr, data_ptr, sizeof(LayerType) * count);                               assert(data_copy_result != nullptr);

#		pragma warning( push )
#		pragma warning( disable : 6201 )
		if (i < count_t - 1)
		{
			u64* parent_layer_branches_realloc_ptr = (u64*)realloc(branches, sizeof(u64) * new_type_count);                    assert(parent_layer_branches_realloc_ptr != nullptr);
			branches = (u64*)parent_layer_branches_realloc_ptr;

			memset(branches + old_type_count, 0, sizeof(u64) * count);
		}
		if (i > 0)
		{
			void* parent_indices_realloc_ptr = realloc(parent_indices, sizeof(u64) * new_type_count);                          assert(parent_indices_realloc_ptr != nullptr);
			parent_indices = (u64*)parent_indices_realloc_ptr;

			u64* parent_indices_dst_ptr = parent_indices + old_type_count;
			btl::memory_set(parent_indices_dst_ptr, parent_branch_index, new_type_count);									   assert(layer_parent_indices[0] == nullptr);
			                                                                                                                   assert(parent_branch_index < layer_sizes[i - 1]);
			u64* parent_branch_ptr = layer_branches[i - 1] + parent_branch_index;                                              assert(layer_branches[i - 1] != nullptr);
			*parent_branch_ptr += count;
		}
#		pragma warning( pop )
	}

	template <class StartArgs, class... Args>
	template <class ParentLayerType, class LayerType> const ParentLayerType& tree<StartArgs, Args...>::get_parent(const LayerType& tree_val) const
	{
		static_assert(std::is_same<LayerType, ParentLayerType>::value == false, "do not attempt to get the equal type parent of tree_val type");
		static_assert(std::is_same<pack_t, LayerType>::value == false, "do not this tree's parameter pack into get_parent");
		static_assert(std::is_same<LayerType, tree<StartArgs, Args...>>::value == false, "do not pass a tree of this tree type into get_parent");
		static_assert(pack_t::alias::contains<ParentLayerType>::value, "tree does not contain ParentLayerType");
		static_assert(pack_t::alias::contains<LayerType>::value, "tree does not contain LayerType");
		static_assert(pack_t::alias::index<ParentLayerType>::value < pack_t::alias::index<LayerType>::value, "specified ParentLayerType must be a parent type of tree_val");
		const constexpr u64 this_layer = pack_t::alias::index<LayerType>::value;
		const constexpr u64 parent_layer = pack_t::alias::index<ParentLayerType>::value;

		u64 current_index = value_index(tree_val);                                                                             assert(current_index < layer_sizes[this_layer]);
		                                                                                                                       assert(layer_parent_indices[this_layer] != nullptr);
		u64* parent_indices_ptr = layer_parent_indices[this_layer];		                                                       assert(this_layer != 0);
		u64 parent_index = *(parent_indices_ptr + current_index);                                                              assert(parent_index < layer_sizes[parent_layer]);

		ParentLayerType* parent_ptr = reinterpret_cast<ParentLayerType*>(layer_data[parent_layer]) + parent_index;             assert(parent_ptr != nullptr);

		return *parent_ptr;
	}


	template <class StartArgs, class... Args>
	void tree<StartArgs, Args...>::print_conceptual() const
	{
		for (u64 i = 0; i < count_t - 1; i++)
		{
			ben::printf("layer %u:\n", i);
			if (layer_sizes[i] > 0)
			{
				ben::printf("{");
				for (u64 j = 0; j < layer_sizes[i]; j++)
				{
					ben::str120 fmt = j == layer_sizes[i] - 1 ? "%llu" : "%llu, ";
					u64 branch_value = *(layer_branches[i] + j);
					ben::printf(fmt, branch_value);
				}
				ben::printf("}\n");
			}
		}
	}
}