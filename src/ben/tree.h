#pragma once

#include "type.h"
#include "memory.h"

namespace btl
{
	// x----------------------------------------------------------------------------------------------x
	// |   - btl::branch points to parent branch as well as maintaining it's index within the layer   |
	// |     and amount of children branches it points to                                             |
	// x----------------------------------------------------------------------------------------------x
	//
	template <typename parent_t, typename child_t>
	struct branch
	{
		u64 index = 0;
		u64 size = 0;
		parent_t* parent = nullptr;
		child_t* children = nullptr;

		branch(parent_t* parent);
		void add_branch_child(const child_t* new_children, u64 count = 1);
		void set_branch_child(const child_t* new_children, u64 size = 1);
	};

	template <typename parent_t, typename child_t>
	branch<parent_t, child_t>::branch(parent_t* parent)
		: parent(parent)
	{}

	template <typename parent_t, typename child_t>
	void branch<parent_t, child_t>::add_branch_child(const child_t* new_children, u64 count)
	{
		size += count;

		child_t* allocation = btl::reallocate(children, size);

		if (allocation == nullptr)
			return nullptr;//print something

		children = allocation;
		btl::memory_copy(child_data + size - count, new_children, count);

		return children;
	}

	template <typename parent_t, typename child_t>
	void branch<parent_t, child_t>::set_branch_child(const child_t* new_children, u64 size)
	{
		this->size = size;

		if (new_children == nullptr)
		{
			free(children);
			children = nullptr;
			return nullptr;
		}

		child_t* allocation = btl::reallocate(children, size);

		if (allocation == nullptr)
			return nullptr;//print something

		children = allocation;
		btl::memory_copy(children, new_children, size);

		return children;
	}

	// x----------------------------------------------------------------------------------------------x
	// |   - btl::layer maintains a pointer to its actual data and an allocation of conceptual btl-   |
	// |     ::branch structs                                                                         |
	// |   - size is equal to number of data elements and conceptual elements                         |
	// x----------------------------------------------------------------------------------------------x
	//
	template <typename T, class parent_branch_t, class child_branch_t>
	struct layer
	{
		u64 size = 0;
		T* data = nullptr;
		btl::branch<parent_branch_t, child_branch_t>* layer_ptr = nullptr;

		void add(parent_branch_t* parent_branch_ptr, child_branch_t* child_branch_ptr);
		void add_data(T* data, u64 data_size = 1)
	};

	template <typename T, class parent_branch_t, class child_branch_t>
	void layer<T, parent_branch_t, child_branch_t>::add(parent_branch_t* parent_branch_ptr, child_branch_t* child_branch_ptr)
	{
		size++;
		auto tmp_layer_ptr = btl::reallocate(layer_ptr, size);

		if (tmp_layer_ptr == nullptr)
			return;//print something

		layer_ptr = tmp_layer_ptr;

		btl::branch<parent_branch_t, child_branch_t> temp(parent_branch_ptr, child_branch_ptr);

		btl::memory_copy(layer_ptr + size - 1, &temp);
	}
}