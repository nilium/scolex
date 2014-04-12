// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __FSTREAM_HH__
#define __FSTREAM_HH__

#include "scolex_config.hh"
#include "basestream.hh"

#include <cstdio>
#include <string>


namespace scolex
{

/*==============================================================================

  File stream

==============================================================================*/
struct fstream_t
{
  fstream_t() = delete;

  // Open file stream
  fstream_t(const string_t &path, stream_mode_t mode);

  fstream_t(const fstream_t &stream) = delete;
  fstream_t(fstream_t &&stream);

  fstream_t &operator = (const fstream_t &stream) = delete;
  // Closes this stream's current stream and replaces it with another stream.
  fstream_t &operator = (fstream_t &&stream);

  ~fstream_t();

  int read(int num_bytes, void *buffer);
  int write(int num_bytes, const void *buffer);

  bool eof() const;
  int tell() const;
  int seek(int pos, stream_seek_origin_t origin);

private:
  std::FILE *file;
  stream_mode_t mode;

  // Closes the fstream_t. If called after the stream is closed, this does
  // nothing.
  void close();

  bool check_mode_for(stream_mode_t required_mode) const;
};


} // namespace scolex

#endif /* end __FSTREAM_HH__ include guard */
