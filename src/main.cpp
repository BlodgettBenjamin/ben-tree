#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdarg.h>

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
		template <class T> constexpr u64 index() const { return pack_t::alias::index<T>::value; };
		template <class T> u64 size() const { return layer_sizes[index<T>()]; };
		template <class T> T* ptr() { return reinterpret_cast<T*>(layer_data[index<T>()]); };

		tree();
		template <class T> void add(const T* data_ptr, u64 count = 1);
	private:
		u64   layer_sizes[count_t]    = { 0 };
		u64*  layer_branches[count_t] = { 0 };
		void* layer_data[count_t]     = { 0 };
	};

	template <class StartArgs, class... Args>
	tree<StartArgs, Args...>::tree()
	{
		static_assert(count_t > 0);

		for (u64 l = 0; l < count_t; l++)
		{
			auto calloc_result = calloc(1, sizeof(u64));
			assert(calloc_result);

			layer_branches[l] = (u64*)calloc_result;
		}

		//assert(memcmp());
	}

	template <class StartArgs, class... Args>
	template <class T> void tree<StartArgs, Args...>::add(const T* data_ptr, u64 count)
	{
		u64 arg_index = index<T>();
		layer_sizes[arg_index] += count;
		auto realloc_result = realloc(layer_data[arg_index], sizeof(T) * layer_sizes[arg_index]);
		assert(realloc_result);

		layer_data[arg_index] = realloc_result;

		auto cpy_result = memcpy((T*)(layer_data[arg_index]) + layer_sizes[arg_index] - count, data_ptr, sizeof(T) * count);
		assert(cpy_result);
	}

	namespace layer
	{

		// x----------------------------------------------------------------------------------------------x
		// |   - btl::layer::iterator iterates the flat allocations of btl::tree                          |
		// |   - use of btl::make_iterable() is necessary for range based for loops                       |
		// x----------------------------------------------------------------------------------------------x
		//
		template <class tree, typename T>
		class iterator
		{
		public:
			iterator(T* layer_ptr)
				: ptr(layer_ptr)
			{};
			iterator& operator++() { ptr++; return *this; }
			iterator operator++(i32) { iterator it = *this; ++(*this); return it; }
			iterator& operator--() { ptr--; return *this; }
			iterator operator--(i32) { iterator it = *this; --(*this); return it; }
			T& operator[](i32 index) { return *(ptr + index); }
			T* operator->() { return ptr; }
			T& operator*() { return *ptr; }
			bool operator==(const iterator& other) const { return ptr == other.ptr; }
			bool operator!=(const iterator& other) const { return ptr != other.ptr; }
		private:
			T* ptr = nullptr;
		};

		// x----------------------------------------------------------------------------------------------x
		// |   - btl::layer::iterable is necessary for btl::make_iterable to work nicely with btl::tree   |
		// |   - btl::tree does not implement an iterator but this will likely change in the future       |
		// x----------------------------------------------------------------------------------------------x
		// |   - don't try to declare a type of btl::layer::iterable                                      |
		// x----------------------------------------------------------------------------------------------x
		//
		template <typename T, class tree_t>
		class iterable
		{
		public:
			iterable(T* data_ptr, T* data_ptr_end)
				: p0(data_ptr), p1(data_ptr_end)
			{};

			layer::iterator<tree_t, T> begin() { return layer::iterator<tree_t, T>(p0); }
			layer::iterator<tree_t, T> end() { return layer::iterator<tree_t, T>(p1); }
		private:
			T* const p0;
			T* const p1;
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
	template <typename T, class tree_t>
	layer::iterable<T, tree_t> make_iterable(tree_t* tree)
	{
		return layer::iterable<T, tree_t>(tree->ptr<T>(), tree->ptr<T>() + tree->size<T>());
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

	return 0;
}