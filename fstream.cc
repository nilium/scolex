// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "fstream.hh"

#include <iostream>


namespace scolex
{


fstream_t::fstream_t(const string_t &path, stream_mode_t mode_)
: mode(mode_)
{
  const char *fopen_mode = NULL;

  switch (mode_) {
  case STREAM_READWRITE:
    fopen_mode = "wb+";
    break;
  case STREAM_WRITE:
    fopen_mode = "wb";
    break;
  case STREAM_READ:
    fopen_mode = "rb";
    break;
  }

  file = std::fopen(path.c_str(), fopen_mode);

  if (!file) {
    throw std::runtime_error("Could not open file.");
  }
}


void fstream_t::close()
{
  if (file) {
    if (std::fclose(file)) {
      std::cerr << "Error while closing file stream (" << file << ':' << mode << "): " << errno << std::endl;
    }
    file = NULL;
  }
}


fstream_t::~fstream_t()
{
  close();
}


fstream_t::fstream_t(fstream_t &&stream)
: file(NULL)
, mode(stream.mode)
{
  std::swap(file, stream.file);
}


fstream_t &fstream_t::operator = (fstream_t &&stream)
{
  close();
  std::swap(file, stream.file);
  mode = stream.mode;
  return *this;
}


bool fstream_t::check_mode_for(stream_mode_t required_mode) const
{
  return file && ((mode & required_mode) == required_mode);
}


int fstream_t::read(int num_bytes, void *buffer)
{
  if (!check_mode_for(STREAM_READ) || num_bytes < 0) {
    return -1;
  } else if (num_bytes == 0) {
    return 0;
  }
  return int(std::fread(buffer, 1, size_t(num_bytes), file));
}


int fstream_t::write(int num_bytes, const void *buffer)
{
  if (!check_mode_for(STREAM_WRITE) || num_bytes < 0) {
    return -1;
  } else if (num_bytes == 0) {
    return 0;
  }
  return int(std::fwrite(buffer, 1, size_t(num_bytes), file));
}


bool fstream_t::eof() const
{
  return !file || std::feof(file);
}


int fstream_t::tell() const
{
  if (file) {
    return int(std::ftell(file));
  }
  return -1;
}


int fstream_t::seek(int pos, stream_seek_origin_t origin)
{
  if (file) {
    int fseek_origin = 0;

    switch (origin) {
    case STREAM_SEEK_CUR: fseek_origin = SEEK_CUR; break;
    case STREAM_SEEK_SET: fseek_origin = SEEK_SET; break;
    case STREAM_SEEK_END: fseek_origin = SEEK_END; break;
    }

    if (std::fseek(file, long(pos), fseek_origin)) {
      return -1;
    } else {
      return tell();
    }
  }
  return -1;
}


} // namespace scolex
