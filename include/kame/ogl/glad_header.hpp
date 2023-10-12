#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#endif

#if __GNUC__
#pragma GCC diagnostic push
#endif

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif

#include "glad.hpp"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#if __GNUC__
#pragma GCC diagnostic pop
#endif

#if __clang__
#pragma clang diagnostic pop
#endif
