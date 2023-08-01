#pragma once

#include "type.h"
#include "memory.h"

namespace btl
{
	// btl::branch is a node that takes advantage of templates. it points to a parent branch node
	// and non child node data. it is meant to be used with a data oriented design. it both
	// maintains a pointer to it's specific data as well as a 64 bit index. this is perhaps
	// redundant and the index might be removed altogether in favor of simply calculating a
	// difference using the child_data pointer but memory use is not a concern for this class.
	// it is meant for use in a dynamic tree structure which will lend itself well to 
	// serialization and use with data oriented design. in practice the branches within the same
	// layers will point to entirely different allocations prior to serialization. during
	// serialization the tree structure can probe the branches to determine a fixed total
	// allocation and the entire tree with the conceptual branches and layer allocations can be
	// allocated in bulk. no protocol for a non-linear resizing of memory is used because after
	// serialization the tree is not intended for applications which require lots of resizing.
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

		child_t* allocation = (child_t*)realloc(child_data, sizeof(child_t) * size);

		if (!allocation)
			return nullptr;//print something

		child_data = allocation;
		memcpy(child_data + size - count, new_child_data, sizeof(child_t) * count);

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

		const u64 allocation_size = sizeof(child_t) * size;
		child_t* allocation = (child_t*)realloc(child_data, allocation_size);

		if (!allocation)
			return nullptr;//print something

		child_data = allocation;
		memcpy(child_data, new_child_data, allocation_size);

		return child_data;
	}
}