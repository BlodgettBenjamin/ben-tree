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
	if (!color_data)
		color_data = position_branch.add_child(&new_color);

	assert(color_data == position_branch.child_data);
	
	return 0;
}