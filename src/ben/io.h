#pragma once

#include <stdarg.h>

#include "type.h"

namespace ben
{
	i32 print(const char* format, ...)
	{
		va_list arg;
		i32 done;

		va_start(arg, format);
		done = vfprintf(stdout, format, arg);
		va_end(arg);

		return done;
	}
#ifdef _BEN_STRING
	i32 print(ben::str120 format, ...)
	{
		va_list arg;
		i32 done;

		va_start(arg, format);
		done = vfprintf(stdout, (const char*)format, arg);
		va_end(arg);

		return done;
	}
#endif
}