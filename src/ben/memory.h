#pragma once

#include <cassert>

#include "type.h"

#define BEN_COUNTOF(array) (sizeof(array) / sizeof(array[0]))

namespace btl
{
	// x----------------------------------------------------------------------------------------------x
	// |   - only disambiguate typename T for void*                                                   |
	// x----------------------------------------------------------------------------------------------x
	//
	template <typename T> inline u64 pointer_offset(const T* pointer1, const T* pointer2);
	template <typename T> inline void* reallocate(T* ptr, u64 new_count);
	template <typename T> inline void* memory_copy(T* dst_ptr, const T* src_ptr, u64 count_t = 1);
	template <typename T> inline void* memory_set(T* dst_ptr, const T& src_val, u64 count_t = 1);
	template <typename T> inline void* resize_allocation(T*& data_ptr, u64 new_count_t);
	template <typename T> inline void* append_allocation(T*& dst_ptr, const T* src_ptr, u64 old_count_t, u64 new_count_t = 1);
	template <typename T> inline void* append_set_allocation(T*& dst_ptr, const T& src_val, u64 old_count_t, u64 new_count_t = 1);
	template <typename T> inline void* append_allocation0(T*& dst_ptr, u64 old_count_t, u64 new_count_t = 1);
	template <typename T> inline void* replicate_allocation(T*& dst_ptr, const T* src_ptr, u64 new_count_t);
}

template <typename T>
inline u64 btl::pointer_offset(const T* pointer1, const T* pointer2)
{
	return pointer2 - pointer1;
}

template <typename T>
inline void* btl::reallocate(T* ptr, u64 new_count)
{
	T* tmp_ptr = (T*)realloc(ptr, sizeof(T) * new_count);
	
	if (tmp_ptr == nullptr)
		return nullptr;//print something

	ptr = tmp_ptr;

	return tmp_ptr;
}

template <typename T>
inline void* btl::memory_copy(T* dst_ptr, const T* src_ptr, u64 count_t)
{
	if (memcpy(dst_ptr, src_ptr, sizeof(T) * count_t) == nullptr)
		return nullptr;//cry
	return dst_ptr;
}

template <typename T>
inline void* btl::memory_set(T* dst_ptr, const T& src_val, u64 count_t)
{
	for (u64 i = 0; i < count_t; i++)
		memcpy(dst_ptr + i, &src_val, sizeof(T));
	assert(memcmp(dst_ptr, dst_ptr + count_t / 2, sizeof(T) * count_t / 2) == 0);
	return dst_ptr;
}

template <typename T>
inline void* btl::resize_allocation(T*& data_ptr, u64 count_t)
{
	void* new_alloc = realloc(data_ptr, sizeof(T) * count_t);
	assert(new_alloc != nullptr);
	data_ptr = (T*)new_alloc;
	return data_ptr;
}

template <typename T>
inline void* btl::append_allocation(T*& dst_ptr, const T* src_ptr, u64 old_count_t, u64 new_count_t)
{
#   ifndef NDEBUG
	if (dst_ptr == nullptr)
		assert(old_count_t == 0);
#   endif
	assert(src_ptr != nullptr);

	btl::resize_allocation(dst_ptr, old_count_t + new_count_t);
	T* copy_offset_ptr = (T*)(dst_ptr) + old_count_t;
	memcpy(copy_offset_ptr, src_ptr, sizeof(T) * new_count_t);
	assert(copy_offset_ptr != nullptr);
	return dst_ptr;
}

template <typename T>
inline void* btl::append_set_allocation(T*& dst_ptr, const T& src_val, u64 old_count_t, u64 new_count_t)
{
#   ifndef NDEBUG
	if (dst_ptr == nullptr)
		assert(old_count_t == 0);
#   endif

	btl::resize_allocation(dst_ptr, new_count_t);
	btl::memory_set(dst_ptr + old_count_t, src_val, new_count_t - old_count_t);
	return dst_ptr;
}

template <typename T>
inline void* btl::append_allocation0(T*& dst_ptr, u64 old_count_t, u64 new_count_t)
{
#   ifndef NDEBUG
	if (dst_ptr == nullptr)
		assert(old_count_t == 0);
#   endif

	btl::resize_allocation(dst_ptr, new_count_t);
	memset(dst_ptr + old_count_t, 0, sizeof(T) * (new_count_t - old_count_t));
	return dst_ptr;
}

template <typename T>
inline void* btl::replicate_allocation(T*& dst_ptr, const T* src_ptr, u64 new_count_t)
{
	btl::resize_allocation(dst_ptr, new_count_t);
	btl::memory_copy(dst_ptr, src_ptr, new_count_t);
	return dst_ptr;
}

namespace ben
{
	u64 bytes_same(const void* data_ptr);
}

u64 ben::bytes_same(const void* data_ptr)
{
	const u8* next = reinterpret_cast<const u8*>(data_ptr) + 1;

	if (memcmp(data_ptr, next, 1) != 0)
		return 1;

	return 1 + bytes_same(next);
}