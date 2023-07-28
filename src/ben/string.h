#pragma once

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)
#include <string.h>

#include "type.h"

namespace ben
{
	static const u64 str128_buffer_size = 128ui64;
	struct str128
	{
		u8 buffer[str128_buffer_size] = { 0 };
		u8 length = 0;

		str128() = default;
		str128(const char* str);
		str128& operator=(const char* str);
		explicit operator const char* () const;
	};

	str128::str128(const char* str)
	{
		*this = str;
	}

	str128& str128::operator=(const char* str)
	{
		memset(this, 0, str128_buffer_size);
		strcpy(reinterpret_cast<char*>(this), str);
		return *this;
	}

	str128::operator const char* () const
	{
		return reinterpret_cast<const char*>(this);
	}
}