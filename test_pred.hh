// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

/**
 * Logical test classes that don't assume both sides of an operator are the
 * same type.
 *
 * test_equal           ->    ==
 * test_not_equal       ->    !=
 * test_less            ->    <
 * test_less_equal      ->    <=
 * test_greater         ->    >
 * test_greater_equal   ->    >=
 */

struct test_equal
{
  template <typename L, typename R>
  bool operator () (L &&lhs, R &&rhs) const
  {
    return lhs == rhs;
  }
};


struct test_not_equal
{
  template <typename L, typename R>
  bool operator () (L &&lhs, R &&rhs) const
  {
    return lhs != rhs;
  }
};


struct test_less
{
  template <typename L, typename R>
  bool operator () (L &&lhs, R &&rhs) const
  {
    return lhs < rhs;
  }
};


struct test_less_equal
{
  template <typename L, typename R>
  bool operator () (L &&lhs, R &&rhs) const
  {
    return lhs <= rhs;
  }
};


struct test_greater
{
  template <typename L, typename R>
  bool operator () (L &&lhs, R &&rhs) const
  {
    return lhs > rhs;
  }
};


struct test_greater_equal
{
  template <typename L, typename R>
  bool operator () (L &&lhs, R &&rhs) const
  {
    return lhs >= rhs;
  }
};
