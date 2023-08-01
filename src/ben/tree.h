#pragma once

#include "type.h"
#include "memory.h"

namespace btl
{
	// btl::branch points to parent branch node and to child DATA. i dont think this makes
	// sense and needs to be reworked to be useful most likely.
	template <typename parent_t, typename child_t>
	struct branch
	{
		u64 index = 0;
		u64 size = 0;
		parent_t* parent = nullptr;
		child_t* child_data = nullptr;

		branch(parent_t* parent, child_t* child_data);
		~branch();
		child_t* add_child(const child_t* new_child_data, u64 count = 1);
		child_t* set_child(const child_t* new_child_data, u64 size = 1);
	};

	template <typename parent_t, typename child_t>
	branch<parent_t, child_t>::branch(parent_t* parent, child_t* child_data)
		: parent(parent), child_data(child_data)
	{}

	template <typename parent_t, typename child_t>
	branch<parent_t, child_t>::~branch()
	{
		parent = nullptr;
		child_data = nullptr;
	}

	template <typename parent_t, typename child_t>
	child_t* branch<parent_t, child_t>::add_child(const child_t* new_child_data, u64 count)
	{
		size += count;

		child_t* allocation = btl::reallocate(child_data, size);

		if (!allocation)
			return nullptr;//print something

		child_data = allocation;
		btl::memory_copy(child_data + size - count, new_child_data, count);

		return child_data;
	}

	template <typename parent_t, typename child_t>
	child_t* branch<parent_t, child_t>::set_child(const child_t* new_child_data, u64 size)
	{
		this->size = size;

		if (!new_child_data)
		{
			free(child_data);
			child_data = nullptr;
			return nullptr;
		}

		child_t* allocation = btl::reallocate(child_data, size);

		if (!allocation)
			return nullptr;//print something

		child_data = allocation;
		btl::memory_copy(child_data, new_child_data, size);

		return child_data;
	}
}