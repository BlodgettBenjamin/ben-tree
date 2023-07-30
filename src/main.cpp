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
	vec3* position_data = nullptr;
	color* color_data = nullptr;
	ben::str120* string_data = nullptr;

	btl::branch<void, color> position_branch(nullptr, color_data);
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