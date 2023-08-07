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
		template <class LayerType> void add(const LayerType* data_ptr, u64 count = 1);
	private:
		u64   layer_sizes[count_t]    = { 0 };
		u64*  layer_branches[count_t] = { 0 };
		void* layer_data[count_t]     = { 0 };
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

		for (u64 l = 0; l < count_t; l++)
		{
			auto calloc_result = calloc(1, sizeof(u64));
			assert(calloc_result);

			layer_branches[l] = (u64*)calloc_result;
		}

		//assert(memcmp());
	}

	template <class StartArgs, class... Args>
	template <class LayerType> void tree<StartArgs, Args...>::add(const LayerType* data_ptr, u64 count)
	{
		u64 arg_index = index<LayerType>();
		layer_sizes[arg_index] += count;
		auto realloc_result = realloc(layer_data[arg_index], sizeof(LayerType) * layer_sizes[arg_index]);
		assert(realloc_result);

		layer_data[arg_index] = realloc_result;

		auto cpy_result = memcpy((LayerType*)(layer_data[arg_index]) + layer_sizes[arg_index] - count, data_ptr, sizeof(LayerType) * count);
		assert(cpy_result);
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
		"obama", "poop", "gaming"
	};
	tree.add(vector_data, BTL_COUNTOF(vector_data));
	tree.add(color_data, BTL_COUNTOF(color_data));
	tree.add(str_data, BTL_COUNTOF(str_data));

	ben::stru64 info_buffer_vec3;
	ben::stru64 info_buffer_color;
	ben::stru64 info_buffer_str;

	info_buffer_vec3.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree.index<vec3>(), tree.size<vec3>(), sizeof(vec3));
	for (const auto& vector : btl::make_iterable<vec3>(&tree))
		info_buffer_vec3.catf("{ %.1f, %.1f, %.1f }\n", vector.x, vector.y, vector.z);

	info_buffer_color.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree.index<color>(), tree.size<color>(), sizeof(color));
	for (const auto& color : btl::make_iterable<color>(&tree))
		info_buffer_color.catf("{ %.1f, %.1f, %.1f, %.1f }\n", color.r, color.g, color.b, color.a);

	info_buffer_str.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree.index<ben::str120>(), tree.size<ben::str120>(), sizeof(ben::str120));
	for (const auto& str : btl::make_iterable<ben::str120>(&tree))
		info_buffer_str.catf("{ %s }\n", str);

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

	count  = pog::count;
	index  = pog::index<obama>;
	is_set = pog::is_set ? "true" : "false";
	ben::printf("\n<poop, obama, gaming>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);
	count  = poo::count;
	index  = poo::index<obama>;
	is_set = poo::is_set ? "true" : "false";
	ben::printf("<poop, obama, obama>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);
	count  = pgpgpo::count;
	index  = pgpgpo::index<obama>;
	is_set = pgpgpo::is_set ? "true" : "false";
	ben::printf("<poop, gaming, poop, gaming, poop, obama>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);
	count  = gg::count;
	index  = gg::index<obama>;
	is_set = gg::is_set ? "true" : "false";
	ben::printf("<gaming, gaming>\nsize:%llu index of obama:%d is set?:%s\n\n", count, index, is_set);

	btl::tree<vec3, color, ben::str120> good_tree;
	//btl::tree<vec3, color, ben::str120*>            bad_tree; (no layers of pointers)
	//btl::tree<vec3, color, ben::stru64>             bad_tree; (cant check for trivial serializeable at compile time so this will compile)
	//btl::tree<vec3, color, std::vector<ben::str120> bad_tree; (std::vector is clearly not trivially copyable so this wont compile)
	if (std::is_trivially_copyable<ben::stru64>::value)
	{
		ben::stru64 str0, str1;
		str0.catf("A trivially copyable type is a type whose storage is contiguous");
		memcpy(&str1, &str0, sizeof(ben::stru64));
		assert(memcmp(&str1, &str0, sizeof(ben::stru64)) == 0);
	}

	return 0;
}