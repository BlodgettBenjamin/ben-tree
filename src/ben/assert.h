#pragma once

#include <cassert>
#include <exception>

#include "io.h"

#ifndef NDEBUG
#   define BEN_ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            ben::printf("Assertion '%s' failed in %s line %i : %s\n", #condition, __FILE__, __LINE__, message);\
            std::terminate(); \
        } \
    } while (false)
#else
#   define BEN_ASSERT(condition, message) do { } while (false)
#endif