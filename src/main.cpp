#include <string.h>
#include <stdio.h>
#include <cassert>

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

int main()
{
	btl::branch<void, color>* position_layer;
	btl::branch<btl::branch<void, color>, ben::str120>* color_layer;
	btl::branch<btl::branch<btl::branch<void, color>, ben::str120>, void>* string_layer;
	vec3* position_data = nullptr;
	color* color_data = nullptr;
	ben::str120* string_data = nullptr;

	auto tmp_pos = (btl::branch<void, color>*)calloc(1, sizeof(btl::branch<void, color>));
	assert(tmp_pos != 0);
	auto tmp_pos2 = (btl::branch<void, color>*)malloc(sizeof(btl::branch<void, color>));
	assert(tmp_pos2 != 0);

	memset(tmp_pos2, 0, sizeof(btl::branch<void, color>));
	assert(memcmp(tmp_pos, tmp_pos2, sizeof(btl::branch<void, color>)) == 0);

	if (!tmp_pos)
		return -1;//print something

	position_layer = tmp_pos;
	*position_layer = btl::branch<void, color>(nullptr, color_data);
	auto position_branch = *position_layer;

	color new_color;
	color_data = position_branch.add_child(&new_color);
	new_color = { 0.0f, 1.0f, 0.0f, 1.0f };
	color_data = position_branch.add_child(&new_color);
	new_color = { 1.0f, 0.0f, 1.0f, 1.0f };
	color_data = position_branch.add_child(&new_color);
	new_color = { 0.0f, 0.0f, 1.0f, 1.0f };
	color_data = position_branch.add_child(&new_color);

	assert(color_data == position_branch.child_data);

	color_data = (color*)malloc(sizeof(color) * position_branch.size);
	assert(color_data != 0);
	memcpy(color_data, position_branch.child_data, sizeof(color) * position_branch.size);
	assert(memcmp(color_data, position_branch.child_data, position_branch.size) == 0);

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
	assert(memcmp(color_data, position_branch.child_data, sizeof(new_color_data)) == 0);
	
	color_data = tmp;

	assert(color_data == tmp);
	assert(memcmp(color_data, position_branch.child_data, sizeof(new_color_data)) == 0);
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