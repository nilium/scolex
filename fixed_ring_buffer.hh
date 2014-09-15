// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <iterator>
#include <type_traits>
#include <cassert>
#include <cstddef>


/*!
 * A basic POD-only ring buffer containing a fixed (at compile-time) number of
 * elements of type T.
 */
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


  struct iterator
  {
    friend struct fixed_ring_buffer<value_type, _Capacity>;

    using value_type = typename self_type::value_type;
    using reference = value_type const &;
    using pointer = value_type const *;
    using difference_type = ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;


  private:

    fixed_ring_buffer const *_buffer;
    size_t _head;

    iterator(fixed_ring_buffer const *buffer, size_t head)
    : _buffer{buffer}
    , _head{head}
    {
      // nop
    }


  public:

    iterator(const iterator &) = default;
    iterator &operator = (const iterator &) = default;

    iterator &operator ++ ()
    {
      ++_head;
      return *this;
    }

    iterator operator ++ (int)
    {
      iterator const dup = *this;
      ++(*this);
      return dup;
    }

    iterator &operator -- ()
    {
      assert(_head > 0);
      --_head;
      return *this;
    }

    iterator operator -- (int)
    {
      iterator const dup = *this;
      --(*this);
      return dup;
    }

    iterator operator + (difference_type delta) const
    {
      assert(
        delta >= 0
        || (delta < 0 && static_cast<size_t>(-delta) <= _head)
        );
      return { _buffer, _head + delta };
    }

    friend iterator operator + (difference_type lhs, iterator const &rhs)
    {
      return rhs + lhs;
    }

    iterator operator - (difference_type delta) const
    {
      return *this + (-delta);
    }

    iterator &operator -= (difference_type rhs)
    {
      *this = *this - rhs;
      return *this;
    }

    iterator &operator += (difference_type rhs)
    {
      *this = *this + rhs;
      return *this;
    }

    difference_type operator - (iterator const &other) const
    {
      assert(_buffer == other._buffer);
      return static_cast<difference_type>(_head) - static_cast<difference_type>(other._head);
    }

    reference operator * () const
    {
      return (*_buffer)[_head];
    }

    pointer operator -> () const
    {
      return &(*_buffer)[_head];
    }

    reference operator [] (difference_type index) const
    {
      return (*_buffer)[static_cast<difference_type>(_head) + index];
    }

    bool operator == (iterator const &other) const
    {
      return _buffer == other._buffer && _head == other._head;
    }

    bool operator != (iterator const &other) const
    {
      return _buffer != other._buffer || _head != other._head;
    }

    bool operator <= (iterator const &other) const
    {
      return _buffer != other._buffer || _head <= other._head;
    }

    bool operator >= (iterator const &other) const
    {
      return _buffer != other._buffer || _head >= other._head;
    }

    bool operator < (iterator const &other) const
    {
      return _buffer != other._buffer || _head < other._head;
    }

    bool operator > (iterator const &other) const
    {
      return _buffer != other._buffer || _head > other._head;
    }
  };

  using const_iterator = iterator;


  static_assert(
    std::is_pod<value_type>::value,
    "fixed_ring_buffer cannot be used with non-POD types"
    );

  static_assert(
    _Capacity > 0,
    "fixed_ring_buffer must have a non-zero capacity"
    );


private:

  size_t _write = 0;
  size_t _read = 0;
  value_type _storage[_Capacity];


public:

  fixed_ring_buffer() = default;

  /*! Attempts to write a value to the buffer. Returns whether it succeeded. */
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

  /*! Returns the next value in the buffer and advances the read head. */
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

  /*! Returns the next value in the buffer without advancing the read head. */
  value_type peek()
  {
    if (can_read()) {
      size_t const index = _read % _Capacity;
      return _storage[index];
    } else {
      return value_type {};
    }
  }


  /*!
   * Returns whether the read head can be rewound once (i.e., whether a call
   * to rewind() will succeed).
   */
  bool can_rewind() const
  {
    return can_write() && _read > 0;
  }

  /*!
   * Rewinds the last read() operation. This is guaranteed to work if and only
   * if the last read() call was not followed by write() or make_contiguous().
   * If write() was called, it will only work if the buffer did not overwrite
   * the last-read value. This fails if no read() has occurred previously.
   */
  bool rewind()
  {
    if (can_rewind()) {
      _read -= 1;
      return true;
    } else {
      return false;
    }
  }


  /*!
   * Returns a rewind marker for the current read head (i.e., just returns the
   * read head). This can be passed to either can_rewind_to(marker) or
   * rewind_to(marker) to rewind to a specific point in the buffer. Rewind
   * markers are only guaranteed to be valid until either make_contiguous() or
   * write() is called. Any change to the write head may invalidate a rewind
   * marker.
   */
  size_t rewind_marker() const
  {
    return _read;
  }

  /*! Returns whether the buffer can be rewound to the given rewind marker. */
  bool can_rewind_to(size_t const marker) const
  {
    return can_write() && marker <= _write && (_write - marker) <= _Capacity;
  }

  /*!
   * Attempts to rewind the ring buffer back to an earlier read marker, if the
   * marker is in range.
   */
  bool rewind_to(size_t marker)
  {
    if (can_rewind_to(marker)) {
      _read = marker;
      return true;
    } else {
      return false;
    }
  }


  /*! Returns whether the ring buffer can be read from (has elements). */
  bool can_read() const
  {
    return size() > 0;
  }

  /*! Returns whether the ring buffer can be written to (isn't full). */
  bool can_write() const
  {
    return size() < _Capacity;
  }


  /*! Returns the number of elements currently held by the ring buffer. */
  size_t size() const
  {
    assert(_read <= _write);
    return _write - _read;
  }

  /*! Returns the ring buffer's capacity. */
  constexpr size_t capacity() const
  {
    return _Capacity;
  }


  /*! Clears the buffer and resets both its read and write heads. */
  void clear()
  {
    _write = _read = 0;
  }


  /*!
   * Returns whether the ring buffer's contents are contiguous. The buffer is
   * considered contiguous if each element in it is ordered one after the other
   * in memory and does not require wrapping the read head to access.
   */
  bool is_contiguous() const
  {
    return (_read % _Capacity) <= (_write % _Capacity);
  }

  /*!
   * Makes the contents of the ring buffer contiguous if the buffer is not
   * contiguous. If not contiguous, all rewind markers are invalidated.
   *
   * If contiguous, this is a no-op and rewind markers remain valid.
   *
   * @post rewind markers are invalidated if is_contiguous() returned true
   * before make_contiguous() was called.
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


  iterator cbegin() const
  {
    return iterator { this, _read };
  }

  iterator cend() const
  {
    return iterator { this, _write };
  }

  iterator begin() const
  {
    return cbegin();
  }

  iterator end() const
  {
    return cend();
  }


private:

  friend struct self_type::iterator;

  template <typename Integer, typename std::enable_if<std::is_integral<Integer>::value, bool>::type = true>
  value_type const &operator [] (Integer index) const
  {
    return _storage[index % static_cast<Integer>(_Capacity)];
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
