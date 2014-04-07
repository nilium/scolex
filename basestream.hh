// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __SCOLEX_BASESTREAM_HH__
#define __SCOLEX_BASESTREAM_HH__

#include "scolex_config.hh"


#ifndef Q_NETWORK_IS_BIG_ENDIAN
#define Q_NETWORK_IS_BIG_ENDIAN 0
#endif

#ifndef Q_HOST_IS_BIG_ENDIAN
#define Q_HOST_IS_BIG_ENDIAN 0
#endif


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


enum stream_endianness_t
{
  STREAM_ENDIAN_LITTLE = 1,
  STREAM_ENDIAN_BIG = 2,

#if Q_HOST_IS_BIG_ENDIAN
  STREAM_ENDIAN_HOST = STREAM_ENDIAN_BIG,
#else
  STREAM_ENDIAN_HOST = STREAM_ENDIAN_LITTLE,
#endif

#if Q_NETWORK_IS_BIG_ENDIAN
  STREAM_ENDIAN_NETWORK = STREAM_ENDIAN_BIG,
#else
  STREAM_ENDIAN_NETWORK = STREAM_ENDIAN_LITTLE,
#endif
};


struct stream_t
{
  virtual ~stream_t() = 0;

  /*
    Read / write blocks of bytes from a stream. Returns <0 on failure,
    otherwise returns the number of bytes written / read. Endianness does not
    affect these functions.
  */
  virtual int read(int num_bytes, void *buffer);
  virtual int write(int num_bytes, void const *buffer);

  // Returns true if at EOF, otherwise false (may be an error, may be impossible
  // to reach EOF, may just not be at EOF, etc.).
  virtual bool eof() const;
  // Returns -1 on failure, otherwise the absolute position in the stream.
  virtual int tell() const;
  // Returns -1 on failure, otherwise the absolute position in the stream after
  // the seek has been applied.
  virtual int seek(int pos, stream_seek_origin_t origin);
};



/*==============================================================================

  Endianness-friendly read/write utility functions.

==============================================================================*/

/*
  When specializing q_write and q_read for non-trivial types, the endianness
  must be forwarded on to inner calls to q_read/write.

  In addition, both forms of q_read should be implemented.
*/

template <class T>
int q_write(stream_t &stream, T const &t_inst, stream_endianness_t endianness = STREAM_ENDIAN_NETWORK);

template <class T>
int q_read(stream_t &stream, T &t_inst, stream_endianness_t endianness = STREAM_ENDIAN_NETWORK);

template <class T>
T q_read(stream_t &stream, stream_endianness_t endianness = STREAM_ENDIAN_NETWORK);


// big/little/host/network-endian specific functions
template <class T>
int q_write_le(stream_t &stream, T const &t_inst)
{
  return q_write<T>(stream, t_inst, STREAM_ENDIAN_LITTLE);
}


template <class T>
int q_write_be(stream_t &stream, T const &t_inst)
{
  return q_write<T>(stream, t_inst, STREAM_ENDIAN_BIG);
}


template <class T>
int q_write_ne(stream_t &stream, T const &t_inst)
{
  return q_write<T>(stream, t_inst, STREAM_ENDIAN_NETWORK);
}


template <class T>
int q_write_he(stream_t &stream, T const &t_inst)
{
  return q_write<T>(stream, t_inst, STREAM_ENDIAN_HOST);
}


template <class T>
int q_read_le(stream_t &stream, T &t_inst)
{
  return q_read<T>(stream, t_inst, STREAM_ENDIAN_LITTLE);
}


template <class T>
int q_read_be(stream_t &stream, T &t_inst)
{
  return q_read<T>(stream, t_inst, STREAM_ENDIAN_BIG);
}


template <class T>
int q_read_ne(stream_t &stream, T &t_inst)
{
  return q_read<T>(stream, t_inst, STREAM_ENDIAN_NETWORK);
}


template <class T>
int q_read_he(stream_t &stream, T &t_inst)
{
  return q_read<T>(stream, t_inst, STREAM_ENDIAN_HOST);
}


template <class T>
T read_le(stream_t &stream)
{
  return q_read<T>(stream, STREAM_ENDIAN_LITTLE);
}


template <class T>
T read_be(stream_t &stream)
{
  return q_read<T>(stream, STREAM_ENDIAN_BIG);
}


template <class T>
T read_ne(stream_t &stream)
{
  return q_read<T>(stream, STREAM_ENDIAN_NETWORK);
}


template <class T>
T read_he(stream_t &stream)
{
  return q_read<T>(stream, STREAM_ENDIAN_HOST);
}


// Write strings
int q_write_nulstring(stream_t &stream, const char *str, int length = -1, int cstrlen = -1);
int q_write_nulstring(stream_t &stream, std::string const &str, int length = -1);

std::string q_read_nulstring(stream_t &stream, int length = -1);


template <class T>
int q_write(stream_t &stream, T const &t_inst, stream_endianness_t endianness)
{
  static_assert(std::is_trivial<T>::value,
    "q_write default implementation only accepts trivially copyable types.");

  if (sizeof(T) <= 1 || STREAM_ENDIAN_HOST == endianness) {
    return stream.write(int(sizeof(T)), &t_inst);
  } else {
    uint8_t const *ptr = (uint8_t *)&t_inst;
    for (int index = sizeof(T) - 1; index >= 0; index -= 1) {
      if (stream.write(1, &ptr[index]) != 1) {
        return int(sizeof(T)) - (index + 1);
      }
    }
    return int(sizeof(T));
  }
}


template <class T>
int q_read(stream_t &stream, T &t_inst, stream_endianness_t endianness)
{
  static_assert(std::is_trivial<T>::value,
    "q_read default implementation only accepts trivially copyable types.");

  if (sizeof(T) <= 1 || STREAM_ENDIAN_HOST == endianness) {
    return stream.read(int(sizeof(T)), &t_inst);
  } else {
    uint8_t *ptr = (uint8_t *)&t_inst;
    for (int index = sizeof(T) - 1; index >= 0; index -= 1) {
      if (stream.read(1, &ptr[index]) != 1) {
        return int(sizeof(T)) - (index + 1);
      }
    }
    return int(sizeof(T));
  }
}


template <class T>
T q_read(stream_t &stream, stream_endianness_t endianness)
{
  static_assert(std::is_trivial<T>::value,
    "q_read default implementation only accepts trivially copyable types.");

  T result {};
  if (q_read(stream, result, endianness) != sizeof result) {
    throw std::runtime_error("Failed to read object from stream.");
  }
  return result;
}


} // namespace scolex

#endif /* end __SCOLEX_BASESTREAM_HH__ include guard */
