// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __SCOLEX_STREAM_ENUMS_HH__
#define __SCOLEX_STREAM_ENUMS_HH__

#include "endian.hh"


namespace scolex
{


enum stream_mode_t
{
  STREAM_READ = 0x1 << 1,
  STREAM_WRITE = 0x1 << 2,

  STREAM_READWRITE = STREAM_READ | STREAM_WRITE,
};


enum stream_seek_origin_t
{
  STREAM_SEEK_SET,
  STREAM_SEEK_CUR,
  STREAM_SEEK_END,
};


} // namespace scolex

#endif /* end __SCOLEX_STREAM_ENUMS_HH__ include guard */
