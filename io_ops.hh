// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __IO_OPS_HH__
#define __IO_OPS_HH__


#include "scolex_config.hh"
#include "stream_enums.hh"


namespace scolex
{
namespace io
{


template <typename STREAM>
class stream_has_eof_t__;


template <class STREAM, bool B>
using eof_bool_t__ = typename std::enable_if<
  stream_has_eof_t__<STREAM>::value == B,
  bool
  >::type;


// Writes num_bytes from input_buffer to stream.
// Returns < 0 on failure.
template <class STREAM>
int write(STREAM &stream, int num_bytes, void const *input_buffer);

// Reads num_bytes from stream to the output_buffer.
// Returns < 0 on failure.
template <class STREAM>
int read(STREAM &stream, int num_bytes, void *output_buffer);

// Returns the current absolute position of read/write ops in the stream.
// Returns < 0 on failure.
template <class STREAM>
int tell(STREAM const &stream);

// Seeks to the given offset in the stream relative to origin.
// If successful, returns the current absolute position of read/write ops in
// the stream.
// Returns < 0 on failure.
template <class STREAM>
int seek(STREAM &stream, int offset, stream_seek_origin_t origin);

// Returns whether the stream is at its EOF.
// If the stream does not implement eof(), this is always false.
// template <class STREAM>
// bool eof(STREAM const &stream);


template <class STREAM>
int write(STREAM &stream, int num_bytes, void const *input_buffer)
{
  static_assert(std::is_same<decltype(stream.write(0, nullptr)), int>::value,
    "stream.write must return an int to be compatible with io:: ops");

  if (num_bytes < 0) {
    return -1;
  } else if (num_bytes == 0) {
    return 0;
  }

  return stream.write(num_bytes, input_buffer);
}


template <class STREAM>
int read(STREAM &stream, int num_bytes, void *output_buffer)
{
  static_assert(std::is_same<decltype(stream.read(0, nullptr)), int>::value,
    "stream.read must return an int to be compatible with io:: ops");

  if (num_bytes < 0) {
    return -1;
  } else if (num_bytes == 0) {
    return 0;
  }

  return stream.read(num_bytes, output_buffer);
}


template <class STREAM>
int tell(STREAM const &stream)
{
  return stream.tell();
}


template <class STREAM>
int seek(STREAM &stream, int offset, stream_seek_origin_t origin)
{
  return stream.seek(offset, origin);
}


template <class STREAM>
eof_bool_t__<STREAM, true> eof(STREAM const &stream)
{
  return stream.eof();
}


template <class STREAM>
eof_bool_t__<STREAM, false> eof(STREAM const &stream)
{
  (void)stream;
  return false;
}


template <typename STREAM>
class stream_has_eof_t__
{
  // Based on Stack Overflow answer by Johannes Schaub:
  // http://stackoverflow.com/a/264088/457812
  template <typename U, U> struct type_check;
  template <typename _1> static std::true_type check(type_check<bool(STREAM::*)()const, &_1::eof> *);
  template <typename> static std::false_type check(...);
public:
  enum : bool { value = decltype(check<STREAM>(0))::value };
};


/*==============================================================================

  Endianness-friendly read/write utility functions.

==============================================================================*/

/*
  When specializing write and read for non-trivial types, the endianness
  must be forwarded on to inner calls to read/write.

  In addition, both forms of read should be implemented.
*/

template <class T, class STREAM>
int write(STREAM &stream, T const &t_inst, endianness_t endianness = ENDIAN_NETWORK);

template <class T, class STREAM>
int read(STREAM &stream, T &t_inst, endianness_t endianness = ENDIAN_NETWORK);

template <class T, class STREAM>
T read(STREAM &stream, endianness_t endianness = ENDIAN_NETWORK);


// big/little/host/network-endian specific functions
template <class T, class STREAM>
int write_le(STREAM &stream, T const &t_inst)
{
  return write<T>(stream, t_inst, ENDIAN_LITTLE);
}


template <class T, class STREAM>
int write_be(STREAM &stream, T const &t_inst)
{
  return write<T>(stream, t_inst, ENDIAN_BIG);
}


template <class T, class STREAM>
int write_ne(STREAM &stream, T const &t_inst)
{
  return write<T>(stream, t_inst, ENDIAN_NETWORK);
}


template <class T, class STREAM>
int write_he(STREAM &stream, T const &t_inst)
{
  return write<T>(stream, t_inst, ENDIAN_HOST);
}


template <class T, class STREAM>
int read_le(STREAM &stream, T &t_inst)
{
  return read<T>(stream, t_inst, ENDIAN_LITTLE);
}


template <class T, class STREAM>
int read_be(STREAM &stream, T &t_inst)
{
  return read<T>(stream, t_inst, ENDIAN_BIG);
}


template <class T, class STREAM>
int read_ne(STREAM &stream, T &t_inst)
{
  return read<T>(stream, t_inst, ENDIAN_NETWORK);
}


template <class T, class STREAM>
int read_he(STREAM &stream, T &t_inst)
{
  return read<T>(stream, t_inst, ENDIAN_HOST);
}


template <class T, class STREAM>
T read_le(STREAM &stream)
{
  return read<T>(stream, ENDIAN_LITTLE);
}


template <class T, class STREAM>
T read_be(STREAM &stream)
{
  return read<T>(stream, ENDIAN_BIG);
}


template <class T, class STREAM>
T read_ne(STREAM &stream)
{
  return read<T>(stream, ENDIAN_NETWORK);
}


template <class T, class STREAM>
T read_he(STREAM &stream)
{
  return read<T>(stream, ENDIAN_HOST);
}


// Read / write strings
template <class STREAM>
int write_nulstring(STREAM &stream, const char *str, int length = -1, int cstrlen = -1);
template <class STREAM>
int write_nulstring(STREAM &stream, string_t const &str, int length = -1);

template <class STREAM>
string_t read_nulstring(STREAM &stream, int length = -1);


template <class T, class STREAM>
int write(STREAM &stream, T const &t_inst, endianness_t endianness)
{
  static_assert(std::is_trivial<T>::value,
    "write default implementation only accepts trivially copyable types.");

  if (sizeof(T) <= 1 || ENDIAN_HOST == endianness) {
    return io::write(stream, int(sizeof(T)), &t_inst);
  } else {
    uint8_t const *ptr = (uint8_t *)&t_inst;
    for (int index = sizeof(T) - 1; index >= 0; index -= 1) {
      if (::scolex::io::write(stream, 1, &ptr[index]) != 1) {
        return int(sizeof(T)) - (index + 1);
      }
    }
    return int(sizeof(T));
  }
}


template <class T, class STREAM>
int read(STREAM &stream, T &t_inst, endianness_t endianness)
{
  static_assert(std::is_trivial<T>::value,
    "read default implementation only accepts trivially copyable types.");

  if (sizeof(T) <= 1 || ENDIAN_HOST == endianness) {
    return ::scolex::io::read(stream, int(sizeof(T)), &t_inst);
  } else {
    uint8_t *ptr = (uint8_t *)&t_inst;
    for (int index = sizeof(T) - 1; index >= 0; index -= 1) {
      if (::scolex::io::read(stream, 1, &ptr[index]) != 1) {
        return int(sizeof(T)) - (index + 1);
      }
    }
    return int(sizeof(T));
  }
}


template <class T, class STREAM>
T read(STREAM &stream, endianness_t endianness)
{
  static_assert(std::is_trivial<T>::value,
    "read default implementation only accepts trivially copyable types.");

  T result {};
  if (read(stream, result, endianness) != sizeof result) {
    throw std::runtime_error("Failed to read object from stream.");
  }
  return result;
}


template <class STREAM>
int write_nulstring(STREAM &stream, const char *str, int length, int cstrlen)
{
  char const zero = 0;

  if (cstrlen < 0) {
    cstrlen = int(strlen(str));
  }

  if (length < 0) {
    length = cstrlen + 1;
  } else if (length == 0) {
    return 0;
  }

  if (cstrlen > length) {
    cstrlen = length - 1;
  }

  int written = ::scolex::io::write(stream, cstrlen, str);

  while (written < length) {
    if (::scolex::io::write(stream, 1, &zero) == 1) {
      written += 1;
    } else {
      return written;
    }
  }

  return length;
}


template <class STREAM>
int write_nulstring(STREAM &stream, string_t const &str, int length)
{
  return write_nulstring(stream, str.c_str(), length, int(str.size()));
}


template <class STREAM>
string_t read_nulstring(STREAM &stream, int length)
{
  string_t result {};

  if (length == 0) {
    return result;
  } else if (length < 0) {
    char buf = '\0';
    while (!::scolex::io::eof(stream)) {
      ::scolex::io::read(stream, 1, &buf);
      if (buf != '\0') {
        result.push_back(buf);
      }
    }
  } else {
    result.resize(size_t(length), '\0');
    if (::scolex::io::read(stream, length, const_cast<char *>(result.data())) != length) {
      throw std::runtime_error("Failed to read entire string from stream.");
    }

    auto const nul_pos = result.find('0');
    if (nul_pos != string_t::npos) {
      result.resize(nul_pos);
    }
  }

  return result;
}


} // namespace io
} // namespace scolex


#endif /* end __IO_OPS_HH__ include guard */
