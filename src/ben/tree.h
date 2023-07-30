#pragma once

#include <cstdlib>

#include "type.h"

namespace btl
{
	template <typename parent_t, typename child_t>
	struct branch
	{
		u64 index = 0;
		u64 size = 0;
		parent_t* parent = nullptr;
		child_t* child_data = nullptr;

		branch(parent_t* parent, child_t* child_data);
		~branch();
		child_t* add_child(const child_t* child);
	};

	template <typename parent_t, typename child_t>
	branch<parent_t, child_t>::branch(parent_t* parent, child_t* child_data)
		: parent(parent), child_data(child_data)
	{}

	template <typename parent_t, typename child_t>
	branch<parent_t, child_t>::~branch()
	{
		parent = nullptr;
		child_data == nullptr;
	}

	template <typename parent_t, typename child_t>
	child_t* branch<parent_t, child_t>::add_child(const child_t* child)
	{
		if (size == 0)
		{

			child_data = (child_t*)malloc(sizeof(child_t));
		}
		else
		{
			child_data = (child_t*)realloc(child_data, sizeof(child_t) * size + 1);
		}

		memcpy(child_data + size, child, sizeof(child_t));
		size++;

		return child_data;
	}

	/*struct vec3
	{
		
	};
	struct epic_tree
	{

	};*/
}