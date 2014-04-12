// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __SCOLEX_CONFIG_HH__
#define __SCOLEX_CONFIG_HH__

#include <cstdint>
#include <string>
#include <type_traits>


#if !defined(__has_builtin)
# define __has_builtin(X) 0
#endif


#if __clang__
# define Q_FALLTHROUGH() [[clang::fallthrough]]
#elif __has_builtin(__builtin_fallthrough)
# define Q_FALLTHROUGH() __builtin_fallthrough()
#else
# define Q_FALLTHROUGH() /* no fallthrough annotation */
#endif

#define Q_CXX_VERSION_LEVEL 11


namespace scolex
{


using string_t = std::string;


}

#endif /* end __SCOLEX_CONFIG_HH__ include guard */
