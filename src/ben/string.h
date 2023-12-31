#pragma once
#define _BEN_STRING

#include <vcruntime_string.h>
#include <stdarg.h>
#include <cassert>

#include "type.h"

namespace ben
{
	// x----------------------------------------------------------------------------------------------x
	// |   - str120 is a stack allocated string of maximimum length 120                               |
	// |   - the sizeof(str120) should always return 120                                              |
	// |   - does not require explicit conversion to c string (this should change in the future)      |
	// x----------------------------------------------------------------------------------------------x
	// |   - the value pointer of str120 points to its buffer                                         |
	// x----------------------------------------------------------------------------------------------x
	// 
	class str120
	{
	public:
		str120() = default;
		str120(const char* str);
		str120& operator=(const char* str);
		bool operator==(const char* other) const;
		char& operator[](u8);
		operator char* ();
		operator const char* () const;
	private:
		char buffer[120] = { 0 };
	};

	str120::str120(const char* str)
	{
		u8 length = (u8)strlen(str) + 1;                       assert(length <= 120);
		                                                       assert(str != nullptr);
		strcpy(buffer, str);                                   assert(buffer != nullptr);
	}

	str120& str120::operator=(const char* str)
	{                                                          assert(strlen(str) + 1 <= 120);
		memset(buffer, 0, 120);
		strcpy(buffer, str);
		return *this;
	}

	bool str120::operator==(const char* str) const
	{
		return strcmp(buffer, str) == 0;
	}

	char& str120::operator[](u8 index)
	{                                                          assert(index < strlen(buffer));
		return buffer[index];
	}

	str120::operator       char* ()
	{
		return reinterpret_cast<char*>(this);
	}

	str120::operator const char* () const
	{
		return reinterpret_cast<const char*>(this);
	}

	// x----------------------------------------------------------------------------------------------x
	// |   - stru64 is a dynamic heap allocated string                                                |
	// |   - the buffer resizes according to a simple protocol of 1.5x the length                     |
	// |   - requires explicit conversion to c string                                                 |
	// x----------------------------------------------------------------------------------------------x
	// 
	class stru64
	{
	public:
		stru64() = default;
		stru64(const char* str);
		stru64& operator=(const char* str);
		char& operator[](u8);
		explicit operator       char* ();
		explicit operator const char* () const;
		i32 catf(const char* format, ...);
	private:
		u64 length = 0;
		u64 buffer_size = 0;
		char* buffer = nullptr;
	};

	stru64::stru64(const char* str)
	{                                                          assert(str != nullptr);
		buffer_size = length = strlen(str) + 1;
		buffer = (char*)calloc(length, sizeof(u8));            assert(buffer != nullptr);
		strcpy(buffer, str);                                   assert(buffer != nullptr);
	}

	stru64& stru64::operator=(const char* str)
	{                                                          assert(str != nullptr);
		buffer_size = length = strlen(str) + 1;               
		auto ptr = realloc(buffer, length);                    assert(ptr != nullptr);
		buffer = (char*)ptr;                                   assert(buffer != nullptr);
		strcpy(buffer, str);                                   assert(buffer != nullptr);

		return *this;
	}

	char& stru64::operator[](u8 index)
	{                                                          assert(index < length);
		return buffer[index];
	}

	stru64::operator       char* ()
	{
		return (char*)buffer;
	}

	stru64::operator const char* () const
	{
		return (const char*)buffer;
	}


	// x----------------------------------------------------------------------------------------------x
	// |   - catf concatenates a dynamic ben::stru64 buffer                                           |
	// |   - it takes advantage of ben::str120s fixed size and vsnprintf to ensure buffer overflow    |
	// |     does not occur                                                                           |
	// |   - in the future it should call a btl::function for formatting strings that uses paramet-   |
	// |     er packs and the assert for temp_buffer should be a part of ben::str120 functionality    |
	// x----------------------------------------------------------------------------------------------x
	//
	i32 stru64::catf(const char* format, ...)
	{
		va_list arg;
		i32 done;

		ben::str120 temp_buffer;
		va_start(arg, format);
		done = vsnprintf(temp_buffer, 120, format, arg);       assert(done > -1 && done < 120);
		va_end(arg);

		if (buffer == nullptr)
		{
			*this = temp_buffer;
		}
		else
		{
			length += strlen(temp_buffer);
			if (buffer_size < length)
			{
				buffer_size = (u64)(length * 1.5);
				auto ptr = realloc(buffer, buffer_size);
				assert(ptr);
				buffer = (char*)ptr;
				assert(buffer);
			}

			strcat(buffer, temp_buffer);                       assert(buffer);
		}

		return done;
	}
}