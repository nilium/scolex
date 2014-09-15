// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <iterator>
#include <type_traits>
#include <cassert>


template <typename T, size_t _Capacity>
struct fixed_ring_buffer
{
  static constexpr size_t CAPACITY = _Capacity;

  using value_type = typename std::decay<T>::type;
  using insert_type =
    typename std::conditional<
      sizeof(value_type) <= 32,
      value_type,
      value_type const &
    >::type;
  using self_type = fixed_ring_buffer<value_type, _Capacity>;

  static_assert(
    std::is_pod<value_type>::value,
    "fixed_ring_buffer cannot be used with non-POD types"
    );

  static_assert(
    _Capacity > 0,
    "fixed_ring_buffer must have a non-zero capacity"
    );


  size_t _write = 0;
  size_t _read = 0;
  value_type _storage[_Capacity];


  fixed_ring_buffer() = default;

  bool write(insert_type value)
  {
    size_t const delta = size();

    if (_write == SIZE_MAX) {
      _read = _Capacity + (_read % _Capacity);
      _write = _read + delta;
    } else if (delta == _Capacity) {
      return false;
    }

    size_t const index = _write % _Capacity;
    _storage[index] = value;

    _write += 1;

    return true;
  }

  value_type read()
  {
    if (can_read()) {
      value_type const result = peek();
      _read += 1;
      return result;
    } else {
      return value_type {};
    }
  }

  value_type peek()
  {
    if (can_read()) {
      size_t const index = _read % _Capacity;
      return _storage[index];
    } else {
      return value_type {};
    }
  }


  bool can_rewind() const
  {
    return can_write() && _read > 0;
  }

  bool rewind()
  {
    if (can_rewind()) {
      _read -= 1;
      return true;
    } else {
      return false;
    }
  }


  size_t rewind_marker() const
  {
    return _read;
  }

  bool can_rewind_to(size_t const marker) const
  {
    return can_write() && marker <= _write && (_write - marker) <= _Capacity;
  }

  bool rewind_to(size_t marker)
  {
    if (can_rewind_to(marker)) {
      _read = marker;
      return true;
    } else {
      return false;
    }
  }


  bool can_read() const
  {
    return size() > 0;
  }

  bool can_write() const
  {
    return size() < _Capacity;
  }


  size_t size() const
  {
    assert(_read <= _write);
    return _write - _read;
  }


  void clear()
  {
    _write = _read = 0;
  }


  bool is_contiguous() const
  {
    return (_read % _Capacity) <= (_write % _Capacity);
  }

  /*!
    Makes the contents of the ring buffer contiguous if the buffer is not
    contiguous. If not contiguous, all rewind markers are invalidated.
    If contiguous, this is a no-op and rewind markers remain valid.

    @pre  is_contiguous -> true
    @post rewind markers are invalidated
  */
  void make_contiguous()
  {
    if (is_contiguous()) {
      return;
    }

    size_t const count = size();
    value_type data[_Capacity];
    for (size_t index = 0; index < count; ++index) {
      data[index] = read();
    }

    _read = 0;
    _write = 0;

    for (size_t index = 0; index < count; ++index) {
      write(data[index]);
    }

    assert(_write == count);
  }


  friend class std::back_insert_iterator<self_type>;

  void push_back(insert_type value)
  {
    if (!can_write()) {
      throw std::logic_error("Attempt to insert using push_back when at capacity");
    }

    write(value);
  }
};
