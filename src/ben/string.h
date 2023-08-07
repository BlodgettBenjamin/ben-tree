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
	// |   - the sizeof(str120) should always return 121                                              |
	// |   - it is both null terminated and stores its length                                         |
	// x----------------------------------------------------------------------------------------------x
	// |   - the value pointer of str120 points to its buffer                                         |
	// x----------------------------------------------------------------------------------------------x
	// 
	struct str120
	{
		static const u64 buffer_size = 120ui64;
		u8 buffer[buffer_size] = { 0 };
		u8 length = 0;

		str120() = default;
		str120(const char* str);
		str120& operator=(const char* str);
		u8& operator[](u8);
		explicit operator       char* ();
		explicit operator const char* () const;
	};

	str120::str120(const char* str)
	{
		length = (u8)strlen(str) + 1;
		assert(length <= buffer_size);

		strcpy((char*)this, str);
	}

	str120& str120::operator=(const char* str)
	{
		length = (u8)strlen(str) + 1;
		assert(length <= buffer_size);

		memset(this, 0, buffer_size);
		strcpy((char*)this, str);

		return *this;
	}

	u8& str120::operator[](u8 index)
	{
		assert(index < length);
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
	// x----------------------------------------------------------------------------------------------x
	// 
	struct stru64
	{
		u64 length = 0;
		u64 buffer_size = 0;
		u8* buffer = nullptr;

		stru64() = default;
		stru64(const char* str);
		stru64& operator=(const char* str);
		u8& operator[](u8);
		explicit operator       char* ();
		explicit operator const char* () const;

		i32 catf(const char* format, ...);
	};

	stru64::stru64(const char* str)
	{
		assert(buffer);
		buffer_size = length = strlen((char*)buffer) + 1;
		buffer = (u8*)calloc(length, sizeof(u8));
		assert(buffer);
		strcpy((char*)buffer, str);
		assert(buffer);
	}

	stru64& stru64::operator=(const char* str)
	{
		buffer_size = length = strlen(str) + 1;
		auto ptr = realloc(buffer, length);
		assert(ptr);
		buffer = (u8*)ptr;
		assert(buffer != nullptr);
		strcpy((char*)buffer, str);
		assert(buffer);

		return *this;
	}

	u8& stru64::operator[](u8 index)
	{
		assert(index < length);
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
		done = vsnprintf((char*)temp_buffer, ben::str120::buffer_size, format, arg);
		assert(done > -1 && done < ben::str120::buffer_size);
		va_end(arg);

		if (buffer == nullptr)
		{
			*this = (char*)temp_buffer;
		}
		else
		{
			length += strlen((char*)temp_buffer);
			if (buffer_size < length)
			{
				buffer_size = (u64)(length * 1.5);
				auto ptr = realloc(buffer, buffer_size);
				assert(ptr);
				buffer = (u8*)ptr;
				assert(buffer);
			}

			strcat((char*)buffer, (char*)temp_buffer);
			assert(buffer);
		}

		return done;
	}
}