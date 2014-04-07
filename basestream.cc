// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "basestream.hh"


namespace scolex
{


stream_t::~stream_t()
{
  /* nop */
}


int stream_t::read(int num_bytes, void *buffer)
{
  (void)num_bytes;
  (void)buffer;
  return -1;
}


int stream_t::write(int num_bytes, const void *buffer)
{
  (void)num_bytes;
  (void)buffer;
  return -1;
}


bool stream_t::eof() const
{
  return false;
}


int stream_t::tell() const
{
  return -1;
}


int stream_t::seek(int pos, stream_seek_origin_t origin)
{
  (void)pos;
  (void)origin;
  return -1;
}


int q_write_nulstring(stream_t &stream, const char *str, int length, int cstrlen)
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

  int written = stream.write(cstrlen, str);

  while (written < length) {
    if (stream.write(1, &zero) == 1) {
      written += 1;
    } else {
      return written;
    }
  }

  return length;
}


int q_write_nulstring(stream_t &stream, string_t const &str, int length)
{
  return q_write_nulstring(stream, str.c_str(), length, int(str.size()));
}


string_t q_read_nulstring(stream_t &stream, int length)
{
  string_t result {};

  if (length == 0) {
    return result;
  } else if (length < 0) {
    char buf = '\0';
    while (!stream.eof()) {
      stream.read(1, &buf);
      if (buf != '\0') {
        result.push_back(buf);
      }
    }
  } else {
    result.resize(size_t(length), '\0');
    if (stream.read(length, const_cast<char *>(result.data())) != length) {
      throw std::runtime_error("Failed to read entire string from stream.");
    }

    auto const nul_pos = result.find('0');
    if (nul_pos != string_t::npos) {
      result.resize(nul_pos);
    }
  }

  return result;
}


} // namespace scolex

