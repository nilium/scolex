/*
 *              Copyright Noel Cower 2014.
 *
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __FUNCTION_INFO_HH__
#define __FUNCTION_INFO_HH__


#include <tuple>


template <typename T, typename Q>
struct function_ptr_type_t__
{
};


template <typename T>
struct function_ptr_type_t__<T, std::false_type>
{
  using type = decltype(&T::operator());
};


template <typename T>
struct function_ptr_type_t__<T, std::true_type>
{
  using type = typename std::decay<T>::type;
};


template <typename T>
using function_ptr_type =
  typename function_ptr_type_t__<T, typename std::is_function<T>::type>::type;


/*
  This odd inheritance hack? (I guess) for matching function types is based on
  KennyTM's Stack Overflow answer at http://stackoverflow.com/a/7943765/457812
  It's been sort of expanded to cover more than just lambdas, though, but
  otherwise it's mostly the same idea as his function_traits class. Only major
  difference is the amount of horror above and below the basic idea here.
 */
template <typename T>
struct function_info : public function_info<function_ptr_type<T>>
{
};


template <typename... Args>
struct arg_info
{
  using types = std::tuple<Args...>;

  template <size_t index>
  using type = typename std::tuple_element<index, types>::type;

  static size_t const arity = sizeof...(Args);
};


template <typename CT, typename RT, typename... Args>
struct function_info<RT(CT::*)(Args...) const>
{
  using return_type = RT;
  using type = RT(CT::*)(Args...) const;
  using args = arg_info<Args...>;

  static bool const is_member = true;
  static bool const is_const = true;
  static bool const is_variadic = false;
};


template <typename CT, typename RT, typename... Args>
struct function_info<RT(CT::*)(Args...)>
{
  using return_type = RT;
  using type = RT(CT::*)(Args...);
  using args = arg_info<Args...>;

  static bool const is_member = true;
  static bool const is_const = false;
  static bool const is_variadic = false;
};


template <typename CT, typename RT, typename... Args>
struct function_info<RT(CT::*)(Args......) const>
{
  using return_type = RT;
  using type = RT(CT::*)(Args......) const;
  using args = arg_info<Args...>;

  static bool const is_member = true;
  static bool const is_const = true;
  static bool const is_variadic = true;
};


template <typename CT, typename RT, typename... Args>
struct function_info<RT(CT::*)(Args......)>
{
  using return_type = RT;
  using type = RT(CT::*)(Args......);
  using args = arg_info<Args...>;

  static bool const is_member = true;
  static bool const is_const = false;
  static bool const is_variadic = true;
};


template <typename RT, typename... Args>
struct function_info<RT(*)(Args...)>
{
  using return_type = RT;
  using type = RT(*)(Args...);
  using args = arg_info<Args...>;

  static bool const is_member = false;
  static bool const is_const = false;
  static bool const is_variadic = false;
};


template <typename RT, typename... Args>
struct function_info<RT(*)(Args......)>
{
  using return_type = RT;
  using type = RT(*)(Args......);
  using args = arg_info<Args...>;

  static bool const is_member = false;
  static bool const is_const = false;
  static bool const is_variadic = true;
};


#endif /* end __FUNCTION_INFO_HH__ include guard */
