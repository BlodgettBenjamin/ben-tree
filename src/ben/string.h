#pragma once
#define _BEN_STRING

#include <vcruntime_string.h>

#include "type.h"

namespace ben
{
	static const u64 str120_buffer_size = 120ui64;
	struct str120
	{
		u8 buffer[str120_buffer_size] = { 0 };
		u8 length = 0;

		str120() = default;
		str120(const char* str);
		str120& operator=(const char* str);
		u8& operator[](u8);
		explicit operator const char* () const;
	};

	str120::str120(const char* str)
	{
		*this = str;
	}

	str120& str120::operator=(const char* str)
	{
		memset(this, 0, str120_buffer_size);

		u64 i = 0;
		for (; i < str120_buffer_size; i++)
		{
			if (str[i] == '\0')
				break;
		}
		memcpy(this, str, i);

		return *this;
	}

	u8& str120::operator[](u8 index)
	{
		return buffer[index];
	}

	str120::operator const char* () const
	{
		return reinterpret_cast<const char*>(this);
	}
}