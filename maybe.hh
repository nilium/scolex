/*
 *              Copyright Noel Cower 2014.
 *
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __MAYBE_HH__
#define __MAYBE_HH__


/*==============================================================================

  typed_if_t<bool, T>
  is_void_typed_t<T>

  Aliases around enable_if and is_void respectively. The latter checks if the
  decayed T is void.

==============================================================================*/
template<bool B, typename T>
using typed_if_t = typename std::enable_if<B, T>::type;


template <typename T>
using is_void_typed_t = std::is_void<typename std::decay<T>::type>;


/*==============================================================================

  invalid_exception_type

  Exception type thrown by maybe_t::exception<T>() when an exception cannot be
  returned as a type T.

==============================================================================*/
struct invalid_exception_type : public std::runtime_error
{
  invalid_exception_type() = delete;
  explicit invalid_exception_type(std::string const &what) : std::runtime_error(what) { /* nop */ }
  explicit invalid_exception_type(char const *what) : std::runtime_error(what) { /* nop */ }
};


/*==============================================================================

  maybe_base_t__

  Base class of maybe_t__. Covers exception conversion, rethrowing, and bool
  cast overload for determining if failure is null.

==============================================================================*/
struct maybe_base_t__
{
  std::exception_ptr failure;

  explicit maybe_base_t__(std::exception_ptr ptr) : failure(ptr) { /* nop */ }

  /*============================================================================
    operator bool () const

      Boolean test for maybe_base_t__ -- returns true if this maybe represents
      a success, otherwise false.
  ============================================================================*/
  operator bool () const { return failure == nullptr; }

  /*============================================================================
    exception<T>() const -> T const &

      Converts the failure case exception_ptr to an exception of type T
      (defaults to std::exception) and returns it. If this conversion is
      invalid, throws invalid_exception_type.

      By default, T is std::exception.
  ============================================================================*/
  template <typename U = std::exception>
  U const &exception() const
  {
    try {
      rethrow();
    } catch (U const &e) {
      return e;
    } catch (...) {
      throw invalid_exception_type("exception could not be caught as type U");
    }
  }

  /*============================================================================
    rethrow() const

      Calls std::rethrow_exception with the failure case exception_ptr.
  ============================================================================*/
  [[noreturn]] void rethrow() const
  {
    std::rethrow_exception(failure);
  }
};


/*==============================================================================

  maybe_t__<T>

  Basic maybe_t__ -- defines an optional member success of type optional<T>.

==============================================================================*/
template <typename T>
struct maybe_t__ final : public maybe_base_t__
{
  optional<T> success;

  maybe_t__(std::exception_ptr ptr, optional<T> &&opt)
  : maybe_base_t__(ptr), success(std::forward<optional<T> &&>(opt))
  {
    /* nop */
  }
};


/*==============================================================================

  maybe_t__<void>

  Empty maybe_t__ -- only inherits the failure member from maybe_base_t__.

==============================================================================*/
template <>
struct maybe_t__<void> final : public maybe_base_t__
{
  explicit maybe_t__(std::exception_ptr ptr)
  : maybe_base_t__(ptr)
  {
    /* nop */
  }
};


/*==============================================================================

  maybe_t<T>

  Type alias over maybe_t__ providing a maybe_t__ type with decayed T as its
  type. This tends to help some with compatibility.

==============================================================================*/
template <typename T>
using maybe_t = maybe_t__<typename std::decay<T>::type>;


/*==============================================================================
  maybe<FN>(FN &&fn) -> maybe_t<decltype(fn())>

    Executes fn and returns a maybe_t with the result of the function. If an
    exception was thrown, its success member (if any) has an undefined value
    and its failure member is the exception that was thrown, otherwise returns
    a maybe_t with a defined success member (if any) and a nullptr failure.
==============================================================================*/
template <typename FN>
auto maybe(FN &&fn) noexcept ->
  typed_if_t<is_void_typed_t<decltype(fn())>::value, maybe_t<void>>
{
  try {
    fn();
    return maybe_t<void> { nullptr };
  } catch(...) {
    return maybe_t<void> { std::current_exception() };
  }
}


template <typename FN>
auto maybe(FN &&fn) noexcept ->
  typed_if_t<!is_void_typed_t<decltype(fn())>::value, maybe_t<decltype(fn())>>
{
  using Opt = optional<decltype(fn())>;
  using T = typename Opt::value_type;
  try {
    return maybe_t<T> { nullptr, Opt::make(fn()) };
  } catch(...) {
    return maybe_t<T> { std::current_exception(), Opt::empty() };

  }
}


/*==============================================================================
  exception_is<T>(exception_ptr) -> bool

    Returns true if the exception_ptr ex_ptr is of type T, otherwise false.
==============================================================================*/
template <typename T>
bool exception_is(std::exception_ptr ex_ptr) noexcept
{
  if (!ex_ptr) {
    return false;
  }

  try {
    std::rethrow_exception(ex_ptr);
  } catch (T const &e) {
    return true;
  } catch (...) {
    return false;
  }
}


/*==============================================================================
  exception_what<T>(exception_ptr) -> char const *

    Returns the what() message for ex_ptr's exception. If the exception pointed
    to is nullptr, the result is "Null exception". If ex_ptr cannot be
    converted to T, returns "Invalid exception type".

    By default, T is std::exception.
==============================================================================*/
template <typename T = std::exception>
char const *exception_what(std::exception_ptr ex_ptr) noexcept
{
  if (!ex_ptr) {
    return "Null exception";
  }

  try {
    std::rethrow_exception(ex_ptr);
  } catch (T const &e) {
    return e.what();
  } catch (...) {
    return "Invalid exception type";
  }
}


#endif /* end __MAYBE_HH__ include guard */
