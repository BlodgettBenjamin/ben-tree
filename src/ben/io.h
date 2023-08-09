#pragma once

#include "type.h"

namespace ben
{
#ifdef _INC_STDIO
#	ifdef _INC_STDARG
	i32 printf(const char* format, ...)
	{
		va_list arg;
		i32 done;

		va_start(arg, format);
		done = vfprintf(stdout, format, arg);
		va_end(arg);

		return done;
	}
#		ifdef _BEN_STRING
	i32 printf(ben::str120 format, ...)
	{
		va_list arg;
		i32 done;

		va_start(arg, format);
		done = vfprintf(stdout, (const char*)format, arg);
		va_end(arg);

		return done;
	}
	i32 printf(ben::stru64 format, ...)
	{
		va_list arg;
		i32 done;

		va_start(arg, format);
		done = vfprintf(stdout, (const char*)format, arg);
		va_end(arg);

		return done;
	}
#		endif
#	endif

#	ifdef _BEN_STRING
	i32 print(ben::str120 string)
	{
		return printf(string);
	}
	i32 print(ben::stru64 string)
	{
		return printf(string);
	}
#	endif
#endif
}