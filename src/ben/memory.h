#pragma once

#include <cassert>

#include "type.h"

#define BTL_COUNTOF(array) (sizeof(array) / sizeof(array[0]))

namespace btl
{
	template <typename T> inline T* reallocate(T* ptr, u64 new_count);
	template <typename T> inline void memory_copy(T* dst_ptr, const T* src_ptr, u64 count_t = 1);
}

template <typename T>
inline T* btl::reallocate(T* ptr, u64 new_count)
{
	T* tmp_ptr = (T*)realloc(ptr, sizeof(T) * new_count);
	
	if (tmp_ptr == nullptr)
		return nullptr;//print something

	ptr = tmp_ptr;

	return tmp_ptr;
}

template <typename T>
inline void btl::memory_copy(T* dst_ptr, const T* src_ptr, u64 count_t)
{
	if (memcpy(dst_ptr, src_ptr, sizeof(T) * count_t) == nullptr)
		return;//cry
}

template <typename T>
inline u64 pointer_offset(const T* pointer1, const T* pointer2)
{
	return pointer2 - pointer1;
}