// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __SCOLEX_ENDIAN_HH__
#define __SCOLEX_ENDIAN_HH__

#include <cstdint>


#ifndef Q_NETWORK_IS_BIG_ENDIAN
#define Q_NETWORK_IS_BIG_ENDIAN 0
#endif

#ifndef Q_HOST_IS_BIG_ENDIAN
#define Q_HOST_IS_BIG_ENDIAN 0
#endif

namespace scolex
{


enum endianness_t : uint32_t
{
  ENDIAN_LITTLE  = 1,
  ENDIAN_BIG     = 2,

  #if Q_HOST_IS_BIG_ENDIAN
  ENDIAN_HOST = ENDIAN_BIG,
  #else
  ENDIAN_HOST = ENDIAN_LITTLE,
  #endif

  #if Q_NETWORK_IS_BIG_ENDIAN
  ENDIAN_NETWORK = ENDIAN_BIG,
  #else
  ENDIAN_NETWORK = ENDIAN_LITTLE,
  #endif
};


}

#endif /* end __SCOLEX_ENDIAN_HH__ include guard */
