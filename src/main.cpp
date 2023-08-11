#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdarg.h>

#include <vector>

#include "ben/comment.h"
#include "ben/type.h"
#include "ben/memory.h"
#include "ben/string.h"
#include "ben/io.h"
#include "ben/parpack.h"
//#include "ben/tree.h"

struct vec3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

struct color
{
	float r = 1.0f;
	float g = 0.5f;
	float b = 0.0f;
	float a = 1.0f;
};

namespace btl
{
	template <class StartArgs, class... Args>
	class tree
	{
	public:
		using pack_t = pack<StartArgs, Args...>;
	public:
		static constexpr u64 const count_t = pack_t::count;
		template <class LayerType> constexpr u64 index() const;
		template <class LayerType> u64 size() const;
		template <class LayerType> LayerType* ptr();

		tree();
		// x----------------------------------------------------------------------------------------------x
		// |   - increments layer size                                                                    |
		// |   - adds specified data to corresponding layer allocation                                    |
		// |   - adds conceptual branches of default count 0 to current layer (if it isnt a leaf layer)   |
		// |   - if it has a parent layer, adds new data count to the 0th parent branch (should select)   |
		// x----------------------------------------------------------------------------------------------x
		//
		template <u64 ParentBranch = U64_MAX, class LayerType> void add(const LayerType* data_ptr, u64 count = 1);
		void print_conceptual() const;
	private:
		u64   layer_sizes[count_t]            = { 0 };
		void* layer_data[count_t]             = { 0 };
		u64*  layer_branches[count_t - 1]     = { 0 };
	};

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
	tree<StartArgs, Args...>::tree()
	{
		static_assert(pack_t::is_set, "parameter pack cannot contain duplicates");
		static_assert(pack_t::triviality == count_t, "all pack parameters must be trivially copyable");
		static_assert(pack_t::pointedness == 0, "tree layers cannot be comprised of pointers");
		assert(memcmp(layer_branches, layer_branches + (count_t - 1) / 2, (count_t - 1) / 2) == 0);
	}

	template <class StartArgs, class... Args>
	template <u64 ParentBranch, class LayerType> void tree<StartArgs, Args...>::add(const LayerType* data_ptr, u64 count)
	{
		static_assert(pack_t::alias::contains<LayerType>::value, "tree does not contain specified type");
		static_assert(ParentBranch != 0 || pack_t::alias::index<LayerType>::value != 0, "ParentBranch parameter specified for layer 0 which does not have a parent layer!");

		const constexpr u64 i = pack_t::alias::index<LayerType>::value;
		const constexpr u64 parent_branch_index = ParentBranch == U64_MAX ? 0 : ParentBranch;
		layer_sizes[i] += count;
		const u64& new_type_count = layer_sizes[i];
		void*& data_allocation_ptr = layer_data[i];

		void* data_realloc_ptr = realloc(data_allocation_ptr, sizeof(LayerType) * new_type_count);                             assert(data_realloc_ptr != nullptr);

		data_allocation_ptr = data_realloc_ptr;

		void* data_copy_dst_ptr = reinterpret_cast<LayerType*>(data_allocation_ptr) + new_type_count - count;
		void* data_copy_result = memcpy(data_copy_dst_ptr, data_ptr, sizeof(LayerType) * count);
		
		#pragma warning( push )
		#pragma warning( disable : 6201 )
		if (i < count_t - 1)
		{
			u64* parent_layer_branches_realloc_ptr = (u64*)realloc(layer_branches[i], sizeof(u64) * new_type_count);           assert(parent_layer_branches_realloc_ptr != nullptr);
			layer_branches[i] = (u64*)parent_layer_branches_realloc_ptr;
			
			memset(layer_branches[i] + new_type_count - count, 0, sizeof(u64) * count);

		}
		if (i > 0)
		{                                                                                                                      assert(parent_branch_index < layer_sizes[i - 1]);
			u64* parent_branch_ptr = layer_branches[i - 1] + parent_branch_index;                                              assert(layer_branches[i - 1] != nullptr);
			*parent_branch_ptr += count;
		}
		#pragma warning( pop )
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
}

int main()
{
	btl::tree<vec3, color, ben::str120> tree;

	vec3 vector_data[] = {
		{1.0f, 2.5f, 4.5f}, {9.5f, 2.7f, 9.2f}
	};
	color color_data[] = {
		{1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}
	};
	ben::str120 str_data[] = {
		"stinky", "chungus", "obamanomics", "epic win!11", "yep"
	};
	tree.add(vector_data, BTL_COUNTOF(vector_data));
	tree.add<0>(color_data, BTL_COUNTOF(color_data));
	tree.add<1>(str_data, BTL_COUNTOF(str_data));

	ben::stru64 info_buffer_vec3;
	ben::stru64 info_buffer_color;
	ben::stru64 info_buffer_str;

	info_buffer_vec3.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree.index<vec3>(), tree.size<vec3>(), sizeof(vec3));
	for (const auto& vector : btl::make_iterable<vec3>(&tree))
		info_buffer_vec3.catf("-{ %.1f, %.1f, %.1f }\n", vector.x, vector.y, vector.z);

	info_buffer_color.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree.index<color>(), tree.size<color>(), sizeof(color));
	for (const auto& color : btl::make_iterable<color>(&tree))
		info_buffer_color.catf("-{ %.1f, %.1f, %.1f, %.1f }\n", color.r, color.g, color.b, color.a);

	info_buffer_str.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree.index<ben::str120>(), tree.size<ben::str120>(), sizeof(ben::str120));
	for (const auto& str : btl::make_iterable<ben::str120>(&tree))
		info_buffer_str.catf("-%s\n", str);

	ben::print(info_buffer_vec3);
	ben::print(info_buffer_color);
	ben::print(info_buffer_str);

	struct obama {};
	struct poop {};
	struct gaming {};
	u64 count, index;
	ben::str120 is_set;

	using pog = btl::pack<poop, obama, gaming>;
	using poo = btl::pack<poop, obama, obama>;
	using pgpgpo = btl::pack<poop, gaming, poop, gaming, poop, obama>;
	using gg = btl::pack<gaming, gaming>;

	count  = pog::count;                                                                                                       assert(count == 3);
	index  = pog::index<obama>;                                                                                                assert(index == 1);
	is_set = pog::is_set ? "true" : "false";                                                                                   assert(is_set == "true");
	ben::printf("\n<poop, obama, gaming>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);
	count  = poo::count;                                                                                                       assert(count == 3);
	index  = poo::index<obama>;                                                                                                assert(index == 1);
	is_set = poo::is_set ? "true" : "false";                                                                                   assert(is_set == "false");
	ben::printf("<poop, obama, obama>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);
	count  = pgpgpo::count;                                                                                                    assert(count == 6);
	index  = pgpgpo::index<obama>;                                                                                             assert(index == 5);
	is_set = pgpgpo::is_set ? "true" : "false";                                                                                assert(is_set == "false");
	ben::printf("<poop, gaming, poop, gaming, poop, obama>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);
	count  = gg::count;                                                                                                        assert(count == 2);
	index  = gg::index<obama>;                                                                                                 assert(index == -1);
	is_set = gg::is_set ? "true" : "false";                                                                                    assert(is_set == "false");
	ben::printf("<gaming, gaming>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);

	btl::tree<vec3, color, ben::str120> good_tree;
	//btl::tree<vec3, color, ben::str120*>            bad_tree; (no layers of pointers)
	//btl::tree<vec3, color, ben::stru64>             bad_tree; (cant check for trivial serializeable at compile time so this will compile)
	//btl::tree<vec3, color, std::vector<ben::str120> bad_tree; (std::vector is clearly not trivially copyable so this wont compile)
	if (std::is_trivially_copyable<ben::stru64>::value)
	{
		ben::stru64 str0, str1;
		str0.catf("A trivially copyable type is a type whose storage is contiguous");
		memcpy(&str1, &str0, sizeof(ben::stru64));                                                                             assert(memcmp(&str1, &str0, sizeof(ben::stru64)) == 0);
	}

	tree.print_conceptual();

	return 0;
}