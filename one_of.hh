// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <tuple>
#include <type_traits>


/**
 * operator == one_of(*)
 *
 * e.g., 1 == one_of(1, 2, 3, 4, 5)    -> true
 * e.g., 0 != one_of(1, 2, 3, 4, 5)    -> true
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


struct logical_or_less
{
  template <typename L, typename R>
  bool operator () (L &&lhs, R &&rhs) const
  {
    return lhs < rhs;
  }
};


struct logical_or_less_equal
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


/**
 * Wrapper around std::tuple to ensure operator == (x, one_of_args) is strongly
 * typed.
 */
template <typename... Args>
struct one_of_args
{
  using type = std::tuple<typename std::decay<Args>::type...>;

  type args;

  template <size_t N = 0, typename Test, typename T>
  auto logical_or_l(T const &lhs, Test const &test) const -> typename std::enable_if<(N >= sizeof...(Args)), bool>::type
  {
    return false;
  }

  template <size_t N = 0, typename Test, typename T>
  auto logical_or_l(T const &lhs, Test const &test) const -> typename std::enable_if<(N < sizeof...(Args)), bool>::type
  {
    return test(lhs, std::get<N>(args)) || logical_or_l<N + 1>(lhs, test);
  }

  template <size_t N = 0, typename Test, typename T>
  auto logical_or_r(T const &rhs, Test const &test) const -> typename std::enable_if<(N >= sizeof...(Args)), bool>::type
  {
    return false;
  }

  template <size_t N = 0, typename Test, typename T>
  auto logical_or_r(T const &rhs, Test const &test) const -> typename std::enable_if< (N < sizeof...(Args)), bool>::type
  {
    return test(std::get<N>(args), rhs) || logical_or_r<N + 1>(rhs, test);
  }
};


/**
 * Returns a one_of_args for use with comparison operators.
 */
template <typename... Args>
one_of_args<Args...> one_of(Args &&... args)
{
  return one_of_args<Args...> { std::make_tuple(std::forward<Args>(args)...) };
}


template <typename Lhs, typename... RhsArgs>
bool operator == (Lhs const &lhs, one_of_args<RhsArgs...> &&rhs)
{
  return rhs.logical_or_l(lhs, test_equal{});
}


template <typename Lhs, typename... RhsArgs>
bool operator != (Lhs const &lhs, one_of_args<RhsArgs...> &&rhs)
{
  return rhs.logical_or_l(lhs, test_not_equal{});
}


template <typename Rhs, typename... LhsArgs>
bool operator == (one_of_args<LhsArgs...> &&lhs, Rhs const &rhs)
{
  return lhs.logical_or_r(rhs, test_equal{});
}


template <typename Rhs, typename... LhsArgs>
bool operator != (one_of_args<LhsArgs...> &&lhs, Rhs const &rhs)
{
  return lhs.logical_or_r(rhs, test_not_equal{});
}


template <typename Lhs, typename... RhsArgs>
bool operator < (Lhs const &lhs, one_of_args<RhsArgs...> &&rhs)
{
  return rhs.logical_or_l(lhs, logical_or_less{});
}


template <typename Lhs, typename... RhsArgs>
bool operator <= (Lhs const &lhs, one_of_args<RhsArgs...> &&rhs)
{
  return rhs.logical_or_l(lhs, logical_or_less_equal{});
}


template <typename Rhs, typename... LhsArgs>
bool operator < (one_of_args<LhsArgs...> &&lhs, Rhs const &rhs)
{
  return lhs.logical_or_r(rhs, logical_or_less{});
}


template <typename Rhs, typename... LhsArgs>
bool operator <= (one_of_args<LhsArgs...> &&lhs, Rhs const &rhs)
{
  return lhs.logical_or_r(rhs, logical_or_less_equal{});
}


template <typename Lhs, typename... RhsArgs>
bool operator > (Lhs const &lhs, one_of_args<RhsArgs...> &&rhs)
{
  return rhs.logical_or_l(lhs, test_greater{});
}


template <typename Lhs, typename... RhsArgs>
bool operator >= (Lhs const &lhs, one_of_args<RhsArgs...> &&rhs)
{
  return rhs.logical_or_l(lhs, test_greater_equal{});
}


template <typename Rhs, typename... LhsArgs>
bool operator > (one_of_args<LhsArgs...> &&lhs, Rhs const &rhs)
{
  return lhs.logical_or_r(rhs, test_greater{});
}


template <typename Rhs, typename... LhsArgs>
bool operator >= (one_of_args<LhsArgs...> &&lhs, Rhs const &rhs)
{
  return lhs.logical_or_r(rhs, test_greater_equal{});
}
