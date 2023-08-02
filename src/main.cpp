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
		inline u64 layer_count();
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
	inline u64 tree<layer_count_t>::layer_count()
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

	for (u32 i = 0; i < tree.layer_count(); i++)
	{
		ben::print("layer #%u has a count of %u elements each sized %u bytes\n", i, tree.layer_sizes[i], tree_sizes[i]);

		if (tree.layer_sizes[i] > 0)
		{
			const u8* const layer_byte_data_ptr = reinterpret_cast<u8*>(tree.layers[i]);
			for (const u8* layer_byte_ptr = layer_byte_data_ptr;
				layer_byte_ptr < layer_byte_data_ptr + tree.layer_sizes[i] * tree_sizes[i];
				layer_byte_ptr++)
			{
				ben::print("byte#%hhu : %hhx\n", layer_byte_ptr - layer_byte_data_ptr, *layer_byte_ptr);
			}
		}
	}
	
	return 0;
}