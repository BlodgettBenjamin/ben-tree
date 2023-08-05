#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <cassert>

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
	template <class tree, typename T>
	class tree_iterator
	{
	public:
		tree_iterator(T* layer_ptr)
			: ptr(layer_ptr)
		{};
		tree_iterator& operator++() { ptr++; return *this; }
		tree_iterator operator++(i32) { tree_iterator it = *this; ++(*this); return it; }
		tree_iterator& operator--() { ptr--; return *this; }
		tree_iterator operator--(i32) { tree_iterator it = *this; --(*this); return it; }
		T& operator[](i32 index) { return *(ptr + index); }
		T* operator->() { return ptr; }
		T& operator*() { return *ptr; }
		bool operator==(const tree_iterator& other) const { return ptr == other.ptr; }
		bool operator!=(const tree_iterator& other) const { return ptr != other.ptr; }
	private:
		T* ptr = nullptr;
	};

	template <class StartArgs, class... Args>
	class tree
	{
	private:
		u64 layer_sizes[1 + sizeof...(Args)] = { 0 };
		void* layers[1 + sizeof...(Args)] = { 0 };
	public:
		template <class T> struct layer
		{
			static constexpr u64 const index = btl::pack::index<T, StartArgs, Args...>::value;
			static constexpr u64 const count = 1 + sizeof...(Args);
		};
		template <class T> u64 size() { return layer_sizes[layer<T>::index]; };
		template <class T> T* ptr() { return reinterpret_cast<T*>(layers[layer<T>::index]); };

		tree() = default;
		template <class T> void add(const T* data_ptr, u64 count = 1);
	};

	template <class StartArgs, class... Args>
	template <class T> void tree<StartArgs, Args...>::add(const T* data_ptr, u64 count)
	{
		u64 arg_index = layer<T>::index;
		layer_sizes[arg_index] += count;
		auto realloc_result = realloc(layers[arg_index], sizeof(T) * layer_sizes[arg_index]);
		assert(realloc_result);

		layers[arg_index] = realloc_result;

		auto cpy_result = memcpy((T*)(layers[arg_index]) + layer_sizes[arg_index] - count, data_ptr, sizeof(T) * count);
		assert(cpy_result);
	}

	template <typename T, class tree_t>
	struct iterable
	{
		T* const p0;
		T* const p1;

		iterable(T* data_ptr, T* data_ptr_end)
			: p0(data_ptr), p1(data_ptr_end)
		{};

		tree_iterator<tree_t, T> begin() { return tree_iterator<tree_t, T>(p0); }
		tree_iterator<tree_t, T> end() { return tree_iterator<tree_t, T>(p1); }
	};

	template <typename T, class tree_t>
	iterable<T, tree_t> make_iterable(tree_t* tree)
	{
		return iterable<T, tree_t>(tree->ptr<T>(), tree->ptr<T>() + tree->size<T>());
	}
}

namespace tree_struct_layer
{
	enum tree_struct_layer : u8
	{
		vec3, color, str,
	};
}
int main()
{
	using tree_t = btl::tree<vec3, color, ben::str120>;
	tree_t tree;

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

	info_buffer_vec3.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree_struct_layer::vec3, tree.size<vec3>(), sizeof(vec3));
	for (const auto& vector : btl::make_iterable<vec3>(&tree))
		info_buffer_vec3.catf("{ %.1f, %.1f, %.1f }\n", vector.x, vector.y, vector.z);

	info_buffer_color.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree_struct_layer::color, tree.size<color>(), sizeof(color));
	for (const auto& color : btl::make_iterable<color>(&tree))
		info_buffer_color.catf("{ %.1f, %.1f, %.1f, %.1f }\n", color.r, color.g, color.b, color.a);

	info_buffer_str.catf("layer #%u has a count of %u elements each sized %u bytes\n", tree_struct_layer::str, tree.size<ben::str120>(), sizeof(ben::str120));
	for (const auto& str : btl::make_iterable<ben::str120>(&tree))
		info_buffer_str.catf("{ %s }\n", str);

	ben::print(info_buffer_vec3);
	ben::print(info_buffer_color);
	ben::print(info_buffer_str);
	
	return 0;
}