// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <iterator>
#include <type_traits>
#include <cstring>


template <typename T, size_t _Capacity>
struct fixed_vector
{
  using size_type = size_t;
  using value_type = typename std::decay<T>::type;
  using insert_type =
    typename std::conditional<
      sizeof(value_type) <= 32,
      value_type,
      value_type const &
    >::type;
  using self_type = fixed_vector<value_type, _Capacity>;

  static_assert(
    std::is_pod<value_type>::value,
    "fixed_vector cannot be used with non-POD types"
    );

  static_assert(
    _Capacity > 0,
    "fixed_vector must have a non-zero capacity"
    );


  size_type _size = 0;
  value_type _data[_Capacity];


  fixed_vector() = default;

  size_type size() const
  {
    return _size;
  }

  constexpr size_type capacity() const
  {
    return _Capacity;
  }


  void clear()
  {
    _size = 0;
  }


  value_type shift()
  {
    value_type const out (( _data[0] ));
    if (_size > 0) {
      _size -= 1;
      size_t const bytes = size_t(end()) - size_t(begin() + 1);
      memmove(begin(), begin() + 1, bytes);
    }
    return out;
  }

  bool unshift(insert_type value)
  {
    switch (_size) {
    case _Capacity: {
      return false;
    }

    default: {
      size_t const bytes = size_t((char const *)end() - (char const *)begin());
      memmove(begin() + 1, begin(), bytes);
      // fallthrough
    }

    case 0: {
      _size += 1;
      _data[0] = value;
      return true;
    }
    }
  }


  value_type pop()
  {
    if (_size == 0)
    {
      return _data[0];
    }

    return _data[--_size];
  }

  // If pushing a value would exceed the capacity of the vector, the push
  // overwrites the last value in the vector.
  bool push(insert_type value)
  {
    if (_size < _Capacity) {
      size_type const index = _size;
      _size += 1;
      _data[index] = value;
      return true;
    } else {
      return false;
    }
  }


  bool can_insert(size_type count = 1) const
  {
    // todo: check for overflow? would it ever happen?
    return (_size + count) <= _Capacity;
  }

  bool is_empty() const
  {
    return _size == 0;
  }


  value_type const *cbegin() const { return &_data[0]; }
  value_type const *cend() const { return (&_data[0]) + _size; }

  value_type const *begin() const { return cbegin(); }
  value_type const *end() const { return cend(); }

  value_type *begin() { return &_data[0]; }
  value_type *end() { return (&_data[0]) + _size; }


  template <
    typename Integer,
    typename std::enable_if<std::is_integral<Integer>::value, bool>::type = true
    >
  value_type &operator [] (Integer index)
  {
    return _data[index];
  }

  template <
    typename Integer,
    typename std::enable_if<std::is_integral<Integer>::value, bool>::type = true
    >
  insert_type operator [] (Integer index) const
  {
    return _data[index];
  }


  friend class std::back_insert_iterator<self_type>;

  void push_back(insert_type value)
  {
    if (!can_insert()) {
      throw std::logic_error("Attempt to insert using push_back when at capacity");
    }

    push(value);
  }


  friend class std::front_insert_iterator<self_type>;

  void push_front(insert_type value)
  {
    if (!can_insert()) {
      throw std::logic_error("Attempt to insert using push_front when at capacity");
    }

    unshift(value);
  }
};
