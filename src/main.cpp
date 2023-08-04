#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <cassert>
#include <type_traits>

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
	class pack
	{
	private:
		template <class T, class ArgFinal>
		static inline constexpr u64 index_rec(u64 t_index)
		{
			static_assert(std::is_same<T, ArgFinal>::value == true);
			return t_index;
		}

		template <class T, class Start, class... PackArgs>
		static inline constexpr u64 index_rec(u64 t_index)
		{
			if (std::is_same<T, Start>::value == true)
				return t_index;

			return pack::index_rec<T, PackArgs...>(t_index + 1);
		}
	public:
		template <class T, class PackStart, class... PackArgs>
		static inline constexpr u64 index()
		{
			if (std::is_same<T, PackStart>::value == true)
				return 0;

			return pack::index_rec<T, PackArgs...>(1);
		}
	};
}

namespace btl
{
	template <class T, class... Args>
	struct tree
	{
		u64 layer_sizes[1 + sizeof...(Args)] = { 0 };
		void* layers[1 + sizeof...(Args)] = { 0 };

		tree() = default;
		inline constexpr u64 layer_count();
		template <class ArgT> void add(const ArgT* data_ptr, u64 count = 1);
	};

	template <class T, class... Args>
	inline constexpr u64 tree<T, Args...>::layer_count()
	{
		return 1 + sizeof...(Args);
	}

	template <class T, class... Args>
	template <class ArgT> void tree<T, Args...>::add(const ArgT* data_ptr, u64 count)
	{
		u64 arg_index = btl::pack::index<ArgT, T, Args...>();
		layer_sizes[arg_index] += count;
		auto realloc_result = realloc(layers[arg_index], sizeof(ArgT) * layer_sizes[arg_index]);
		assert(realloc_result);

		layers[arg_index] = realloc_result;

		auto cpy_result = memcpy((ArgT*)(layers[arg_index]) + layer_sizes[arg_index] - count, data_ptr, sizeof(ArgT) * count);
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

	btl::tree<vec3, color, ben::str120> tree;

	vec3 vector_data[] = { {1.0f, 2.5f, 4.5f}, {9.5f, 2.7f, 9.2f} };
	tree.add(vector_data, 2);

	ben::stru64 info_buffer_vec3;
	ben::stru64 info_buffer_color;
	ben::stru64 info_buffer_str;

	u32 layer_index;

	layer_index = tree_struct_layer::vec3;
	info_buffer_vec3.catf("layer #%u has a count of %u elements each sized %u bytes\n", layer_index, tree.layer_sizes[layer_index], sizeof(vec3));
	for (u64 i = 0; i < tree.layer_sizes[layer_index]; i++)
	{
		vec3* vector = (vec3*)(tree.layers[layer_index]) + i;
		info_buffer_vec3.catf("{ %.1f, %.1f, %.1f }\n", vector->x, vector->y, vector->z);
	}

	layer_index = tree_struct_layer::color;
	info_buffer_color.catf("layer #%u has a count of %u elements each sized %u bytes\n", layer_index, tree.layer_sizes[layer_index], sizeof(color));
	for (u64 i = 0; i < tree.layer_sizes[layer_index]; i++)
	{
		color* col = (color*)(tree.layers[layer_index]) + i;
		info_buffer_color.catf("{ %.1f, %.1f, %.1f, %.1f }\n", col->r, col->g, col->b, col->a);
	}

	layer_index = tree_struct_layer::str;
	info_buffer_str.catf("layer #%u has a count of %u elements each sized %u bytes\n", layer_index, tree.layer_sizes[layer_index], sizeof(ben::str120));
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