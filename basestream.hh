// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __SCOLEX_BASESTREAM_HH__
#define __SCOLEX_BASESTREAM_HH__

#include "scolex_config.hh"
#include "stream_enums.hh"


namespace scolex
{


/*
  Stream classes should implement some of the following public methods, though
  all are optional depending on the purpose of the stream class.

  int read(int num_bytes, void *output_buffer)
  int write(int num_bytes, void const *input_buffer)
    Reads from or writes to the stream, depending on the stream's mode. The
    result is the number of bytes read or written if successful, otherwise a
    number < 0. Reads/writes of num_bytes 0 should be no-ops, and this policy
    is followed by io::read/write functions (meaning no function will be called
    if num_bytes is 0). If num_bytes is < 0, this is considered invalid and
    either a number < 0 should be returned or something suitably handle-able
    should happen to indicate an error.

  int tell() const
    Returns the current absolute position of read/write operations in the
    stream. If this fails for some reason, the result should be a number < 0.

  int seek(int offset, stream_seek_origin_t origin)
    Sets the offset of the stream relative to the given origin. Should return
    a number < 0 on failure. If successful, must return the absolute position
    of the stream after the seek (i.e., must return what tell() would return).

  bool eof() const
    Queries whether the stream is at its eof (whatever that may be). Is not
    required to return true at any point for write streams. There is no error
    state for this function. If unimplemented, io::eof will always return false.

*/


struct nulstream_t final
{
  int read(int num_bytes, void *buffer) { (void)num_bytes; (void)buffer; return 0; }
  int write(int num_bytes, void const *buffer) { (void)num_bytes; (void)buffer; return num_bytes; }
  bool eof() { return false; }
  int tell() const { return 0; }
  int seek(int offset, stream_seek_origin_t origin) { (void)offset; (void)origin; return 0; }
};


/*==============================================================================

  stream_wrapper_t

  Defines a wrapper for any class implementing the basic stream interface.

==============================================================================*/
template <class T>
class stream_wrapper_t final
{
  T &underlying;

public:
  stream_wrapper_t(T &ref) : underlying(ref) {}

  stream_wrapper_t(stream_wrapper_t const &other) = default;
  stream_wrapper_t(stream_wrapper_t &&other) = delete;

  stream_wrapper_t &operator == (stream_wrapper_t const &other) = default;
  stream_wrapper_t &operator == (stream_wrapper_t &&other) = delete;

  int read(int num_bytes, void *buffer)
  {
    return underlying.read(num_bytes, buffer);
  }

  int write(int num_bytes, void const *buffer)
  {
    return underlying.write(num_bytes, buffer);
  }

  bool eof() const
  {
    return underlying.eof();
  }

  int tell() const
  {
    return underlying.tell();
  }

  int seek(int pos, stream_seek_origin_t origin)
  {
    return underlying.seek(pos, origin);
  }
};


} // namespace scolex


#include "io_ops.hh"


#endif /* end __SCOLEX_BASESTREAM_HH__ include guard */
