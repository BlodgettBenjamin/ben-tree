#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <cassert>

#include "ben/type.h"
#include "ben/memory.h"
#include "ben/string.h"
#include "ben/io.h"
#include "ben/tree.h"

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

template <typename parent_branch_t, typename child_branch_t>
struct layer
{
	u64 size = 0;
	btl::branch<parent_branch_t, child_branch_t>* layer_ptr = nullptr;

	void add_branch(parent_branch_t* parent_branch_ptr, child_branch_t* child_branch_ptr);
};

template <typename parent_branch_t, typename child_branch_t>
void layer<parent_branch_t, child_branch_t>::add_branch(parent_branch_t* parent_branch_ptr, child_branch_t* child_branch_ptr)
{
	size++;
	auto tmp_layer_ptr = btl::reallocate(layer_ptr, size);

	if (!tmp_layer_ptr)
		return;//print something

	layer_ptr = tmp_layer_ptr;

	btl::branch<parent_branch_t, child_branch_t> temp(parent_branch_ptr, child_branch_ptr);

	btl::memory_copy(layer_ptr + size - 1, &temp);
}

int main()
{

	u64 position_layer_size, color_layer_size, string_layer_size;
	position_layer_size = color_layer_size = string_layer_size = 0;
	layer<void, color>                                                  position_layer;
	layer<btl::branch<void, color>, ben::str120>                        color_layer;
	layer<btl::branch<btl::branch<void, color>, ben::str120>, void>     string_layer;
	vec3* position_data = nullptr;
	color* color_data = nullptr;
	ben::str120* string_data = nullptr;

	position_layer.add_branch(nullptr, color_data);

	auto& position_branch = *position_layer.layer_ptr;

	color new_color;
	color_data = position_branch.add_child(&new_color);
	new_color = { 0.0f, 1.0f, 0.0f, 1.0f };
	color_data = position_branch.add_child(&new_color);
	new_color = { 1.0f, 0.0f, 1.0f, 1.0f };
	color_data = position_branch.add_child(&new_color);
	new_color = { 0.0f, 0.0f, 1.0f, 1.0f };
	color_data = position_branch.add_child(&new_color);

	assert(color_data == position_branch.child_data);

	color_data = (color*)std::malloc(sizeof(color) * position_branch.size);
	assert(color_data != 0);
	std::memcpy(color_data, position_branch.child_data, sizeof(color) * position_branch.size);
	assert(std::memcmp(color_data, position_branch.child_data, position_branch.size) == 0);

	position_branch.set_child(nullptr, 0);

	assert(position_branch.size != 4);
	assert(position_branch.size == 0);

	assert(position_branch.child_data == nullptr);
	assert(color_data != position_branch.child_data);

	color new_color_data[] = {
		color(),
		{ 0.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 1.0f }
	};
	color* tmp = position_branch.set_child(new_color_data, sizeof(new_color_data));

	assert(tmp == position_branch.child_data);
	assert(color_data != position_branch.child_data);
	assert(position_branch.child_data != 0);
	assert(std::memcmp(color_data, position_branch.child_data, sizeof(new_color_data)) == 0);
	
	color_data = tmp;

	assert(color_data == tmp);
	assert(std::memcmp(color_data, position_branch.child_data, sizeof(new_color_data)) == 0);
	assert(position_branch.size == sizeof(new_color_data));

	btl::branch<btl::branch<void, color>, ben::str120> color_branch(&position_branch, string_data);
	ben::str120 new_string;
	string_data = color_branch.add_child(&new_string);

	assert(string_data == color_branch.child_data);
	
	free(position_data);
	free(color_data);
	free(string_data);
	
	return 0;
}