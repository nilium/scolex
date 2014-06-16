#ifndef __MAYBE_HH__
#define __MAYBE_HH__


template<bool B, typename T>
using typed_if_t = typename std::enable_if<B, T>::type;


template <typename T>
using is_void_typed_t = std::is_void<typename std::decay<T>::type>;


struct invalid_exception_type : public std::runtime_error
{
  invalid_exception_type() = delete;
  explicit invalid_exception_type(std::string const &what) : std::runtime_error(what) { /* nop */ }
  explicit invalid_exception_type(char const *what) : std::runtime_error(what) { /* nop */ }
};


struct maybe_base_t__
{
  std::exception_ptr failure;

  explicit maybe_base_t__(std::exception_ptr ptr) : failure(ptr) { /* nop */ }

  operator bool () const { return failure == nullptr; }

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

  [[noreturn]] void rethrow() const
  {
    std::rethrow_exception(failure);
  }
};


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


template <>
struct maybe_t__<void> final : public maybe_base_t__
{
  explicit maybe_t__(std::exception_ptr ptr)
  : maybe_base_t__(ptr)
  {
    /* nop */
  }
};


template <typename T>
using maybe_t = maybe_t__<typename std::decay<T>::type>;


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


template <typename T = std::exception>
char const *exception_what(std::exception_ptr ex_ptr) noexcept
{
  if (!ex_ptr) {
    return "Null exception";
  }

  if (exception_is<T>(ex_ptr)) {
    try {
      std::rethrow_exception(ex_ptr);
    } catch (T const &e) {
      return e.what();
    }
  } else {
    return "Unknown exception type";
  }
}


#endif /* end __MAYBE_HH__ include guard */
