#pragma once

#include <stdarg.h>
#include <stdio.h>

#include "string.h"

namespace btl
{
	i32 print(ben::str120 format, ...)
	{
		va_list arg;
		i32 done;

		va_start(arg, format);
		done = vfprintf(stdout, (const char*)format, arg);
		va_end(arg);

		return done;
	}
}