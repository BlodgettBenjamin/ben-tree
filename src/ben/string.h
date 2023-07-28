#pragma once

#include "type.h"

namespace ben
{
	static const u64 str128_buffer_size = 120ui64;
	struct str120
	{
		u8 buffer[str128_buffer_size] = { 0 };
		u8 length = 0;

		str120() = default;
		str120(const char* str);
		str120& operator=(const char* str);
		explicit operator const char* () const;
	};

	str120::str120(const char* str)
	{
		*this = str;
	}

	str120& str120::operator=(const char* str)
	{
		memset(this, 0, str128_buffer_size);

		u64 i = 0;
		for (; i < str128_buffer_size; i++)
		{
			if (str[i] == '\0')
				break;
		}
		memcpy(this, str, i);

		return *this;
	}

	str120::operator const char* () const
	{
		return reinterpret_cast<const char*>(this);
	}
}