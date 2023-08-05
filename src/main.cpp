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
		tree_iterator& operator++()
		{
			ptr++;
			return *this;
		}
		tree_iterator operator++(i32)
		{
			tree_iterator it = *this;
			++(*this);
			return it;
		}
		tree_iterator& operator--()
		{
			ptr--;
			return *this;
		}
		tree_iterator operator--(i32)
		{
			tree_iterator it = *this;
			--(*this);
			return it;
		}
		T& operator[](i32 index)
		{
			return *(ptr + index);
		}
		T* operator->()
		{
			return ptr;
		}
		T& operator*()
		{
			return *ptr;
		}
		bool operator==(const tree_iterator& other) const
		{
			return ptr == other.ptr;
		}
		bool operator!=(const tree_iterator& other) const
		{
			return ptr != other.ptr;
		}
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
		template <class T> struct layer { 
			static constexpr u64 const index = btl::pack::index<T, StartArgs, Args...>::value;
			static constexpr u64 const count = 1 + sizeof...(Args);
		};
		template <class T> u64 size() { return layer_sizes[layer<T>::index]; };
		template <class T> T* ptr() { return reinterpret_cast<T*>(layers[layer<T>::index]); };

		tree() = default;
		template <class T> void add(const T* data_ptr, u64 count = 1);

		template <class T> tree_iterator<tree<StartArgs, Args...>, T> begin();
		template <class T> tree_iterator<tree<StartArgs, Args...>, T> end();
	};
	
	template <class StartArgs, class... Args>
	template <class T> tree_iterator<tree<StartArgs, Args...>, T> tree<StartArgs, Args...>::begin()
	{
		return tree_iterator<tree<StartArgs, Args...>, T>(ptr<T>());
	}

	template <class StartArgs, class... Args>
	template <class T> tree_iterator<tree<StartArgs, Args...>, T> tree<StartArgs, Args...>::end()
	{
		return tree_iterator<tree<StartArgs, Args...>, T>(ptr<T>() + size<T>());
	}

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

	u32 layer_index;

	layer_index = tree_struct_layer::vec3;
	info_buffer_vec3.catf("layer #%u has a count of %u elements each sized %u bytes\n", layer_index, tree.size<vec3>(), sizeof(vec3));
	for (btl::tree_iterator<tree_t, vec3> it = tree.begin<vec3>(); it != tree.end<vec3>(); it++)
	{
		info_buffer_vec3.catf("{ %.1f, %.1f, %.1f }\n", it->x, it->y, it->z);
	}

	layer_index = tree_struct_layer::color;
	info_buffer_color.catf("layer #%u has a count of %u elements each sized %u bytes\n", layer_index, tree.size<color>(), sizeof(color));
	for (btl::tree_iterator<tree_t, color> it = tree.begin<color>(); it != tree.end<color>(); it++)
	{
		info_buffer_color.catf("{ %.1f, %.1f, %.1f, %.1f }\n", it->r, it->g, it->b, it->a);
	}

	layer_index = tree_struct_layer::str;
	info_buffer_str.catf("layer #%u has a count of %u elements each sized %u bytes\n", layer_index, tree.size<ben::str120>(), sizeof(ben::str120));
	for (btl::tree_iterator<tree_t, ben::str120> it = tree.begin<ben::str120>(); it != tree.end<ben::str120>(); it++)
	{
		info_buffer_str.catf("{ %s }\n", *it);
	}

	ben::print(info_buffer_vec3);
	ben::print(info_buffer_color);
	ben::print(info_buffer_str);
	
	return 0;
}