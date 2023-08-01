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
	template <class sizes_array_t, u64 layer_count_t>
	struct tree
	{
		u64* layer_sizes = nullptr;
		void** layers = nullptr;

		tree();
		void add(u8 layer, const void* data_ptr, u64 count = 1);
	};

	template <class sizes_array_t, u64 layer_count_t>
	tree<sizes_array_t, layer_count_t>::tree()
	{
		layer_sizes = (u64*)calloc(layer_count_t, sizeof(u64));
		
		if (!layer_sizes)
			return;//complain

		layers = (void**)calloc(layer_count_t, sizeof(void*));

		if (!layers)
			return;//complain
	}

	template <class sizes_array_t, u64 layer_count_t>
	void tree<sizes_array_t, layer_count_t>::add(u8 layer_index, const void* data_ptr, u64 count)
	{
		layer_sizes[layer_index] += count;
		layers[layer_index] = realloc(layers[layer_index], layer_sizes[layer_index] * sizes_array_t[layer_index]);

		if (!layers[layer_index])
			return;//complain

		auto cpy_result = memcpy(layers[layer_index] + (layer_sizes[layer_index] - count) * sizes_array_t[layer_index], data_ptr, count * sizes_array_t[layer_index]);
		
		if (!cpy_result)
			return;//complain
	}
}

int main()
{
	enum class tree_struct_layer : u8
	{
		vec3, color, str,
	};
	struct tree_sizes
	{
		const u64 vec3 = sizeof(vec3);
		const u64 color = sizeof(color);
		const u64 str = sizeof(ben::str120);
	};
	btl::tree<tree_sizes, 3> tree;
	
	return 0;
}