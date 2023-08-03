#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <cassert>

#include "ben/type.h"
#include "ben/memory.h"
#include "ben/string.h"
#include "ben/io.h"
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
	template <u64 layer_count_t>
	struct tree
	{
		u64* layer_sizes = nullptr;
		void** layers = nullptr;

		tree();
		inline constexpr u64 layer_count();
		void add(const u64* const layer_type_sizes, u8 layer_index, const void* data_ptr, u64 count = 1);
	};

	template <u64 layer_count_t>
	tree<layer_count_t>::tree()
	{
		layer_sizes = (u64*)calloc(layer_count_t, sizeof(u64));
		
		if (!layer_sizes)
			return;//complain

		layers = (void**)calloc(layer_count_t, sizeof(void*));

		if (!layers)
			return;//complain
	}

	template <u64 layer_count_t>
	inline constexpr u64 tree<layer_count_t>::layer_count()
	{
		return layer_count_t;
	}

	template <u64 layer_count_t>
	void tree<layer_count_t>::add(const u64* const layer_type_sizes, u8 layer_index, const void* data_ptr, u64 count)
	{
		layer_sizes[layer_index] += count;
		auto realloc_result = realloc(layers[layer_index], layer_type_sizes[layer_index] * (layer_sizes[layer_index]));

		if (!realloc_result)
			return;//complain

		layers[layer_index] = realloc_result;

		auto cpy_result = memcpy((u8*)(layers[layer_index]) + (layer_sizes[layer_index] - count) * layer_type_sizes[layer_index], data_ptr, count * layer_type_sizes[layer_index]);
		
		if (!cpy_result)
			return;//complain
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
	u64 tree_sizes[] = { sizeof(vec3), sizeof(color), sizeof(ben::str120) };
	btl::tree<3> tree;

	vec3 vector_data[] = { {1.0f, 2.5f, 4.5f}, {9.5f, 2.7f, 9.2f} };
	tree.add(tree_sizes, tree_struct_layer::vec3, vector_data, 2);

	ben::stru64 info_buffer_vec3;
	ben::stru64 info_buffer_color;
	ben::stru64 info_buffer_str;

	u32 layer_index;

	layer_index = tree_struct_layer::vec3;
	info_buffer_vec3.catf("layer #%u has a count of %u elements each sized %u bytes\n", layer_index, tree.layer_sizes[layer_index], tree_sizes[layer_index]);
	for (u64 i = 0; i < tree.layer_sizes[layer_index]; i++)
	{
		vec3* vector = (vec3*)(tree.layers[layer_index]) + i;
		info_buffer_vec3.catf("{ %f, %f, %f }\n", vector->x, vector->y, vector->z);
	}

	layer_index = tree_struct_layer::color;
	info_buffer_color.catf("layer #%u has a count of %u elements each sized %u bytes\n", layer_index, tree.layer_sizes[layer_index], tree_sizes[layer_index]);
	for (u64 i = 0; i < tree.layer_sizes[layer_index]; i++)
	{
		color* col = (color*)(tree.layers[layer_index]) + i;
		info_buffer_color.catf("{ %f, %f, %f, %f }\n", col->r, col->g, col->b, col->a);
	}

	layer_index = tree_struct_layer::str;
	info_buffer_str.catf("layer #%u has a count of %u elements each sized %u bytes\n", layer_index, tree.layer_sizes[layer_index], tree_sizes[layer_index]);
	for (u64 i = 0; i < tree.layer_sizes[layer_index]; i++)
	{
		ben::str120* string = (ben::str120*)(tree.layers[layer_index]) + i;
		info_buffer_str.catf("{ %s }\n", string);
	}

	ben::print(info_buffer_vec3);
	ben::print(info_buffer_color);
	ben::print(info_buffer_str);
	
	return 0;
}