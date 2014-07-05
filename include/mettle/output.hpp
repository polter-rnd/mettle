#ifndef INC_METTLE_OUTPUT_HPP
#define INC_METTLE_OUTPUT_HPP

#include <locale>
#include <codecvt>
#include <iomanip>
#include <sstream>
#include <type_traits>
#include <typeinfo>

namespace mettle {

namespace detail {
  template<typename Tuple, typename Func, typename Val,
           size_t N = std::tuple_size<Tuple>::value>
  struct do_reduce {
    auto operator ()(const Tuple &tuple, const Func &reducer,
                     Val &&value) {
      constexpr auto i = std::tuple_size<Tuple>::value - N;
      bool early_exit = false;
      auto result = reducer(
        std::forward<Val>(value), std::get<i>(tuple), early_exit
      );
      if(early_exit)
        return result;
      return do_reduce<Tuple, Func, Val, N-1>()(
        tuple, reducer, std::forward<Val>(result)
      );
    }
  };

  template<typename Tuple, typename Func, typename Val>
  struct do_reduce<Tuple, Func, Val, 1> {
    auto operator ()(const Tuple &tuple, const Func &reducer,
                     Val &&value) {
      constexpr auto i = std::tuple_size<Tuple>::value - 1;
      bool early_exit = false;
      return reducer(std::forward<Val>(value), std::get<i>(tuple), early_exit);
    }
  };

  template<typename Tuple, typename Func, typename Val>
  struct do_reduce<Tuple, Func, Val, 0> {
    auto operator ()(const Tuple &, const Func &, Val &&value) {
      return value;
    }
  };

  template<typename Tuple, typename Func, typename Val>
  auto reduce_tuple(const Tuple &tuple, const Func &reducer, Val &&initial) {
    return do_reduce<Tuple, Func, Val>()(
      tuple, reducer, std::forward<Val>(initial)
    );
  }

  template<typename T>
  std::string stringify_iterable(const T &begin, const T &end);

  template<typename T>
  std::string stringify_tuple(const T &tuple);

  inline std::string quoted_temp(
    const std::string &s, char delim = '"', char escape = '\\'
  ) {
    std::stringstream ss;
    ss << std::quoted(s, delim, escape);
    return ss.str();
  }
}

template<typename T>
class is_printable {
  template<typename U> struct always_bool { typedef bool type; };

  template<typename U>
  static constexpr typename always_bool<
    decltype( std::declval<std::ostream&>() << std::declval<U>() )
  >::type check_(int) {
    return true;
  }
  template<typename U>
  static constexpr bool check_(...) {
    return false;
  }
public:
  static const bool value = check_<T>(0);
};

template<typename T>
struct is_boolish : std::integral_constant<bool,
  std::is_same<typename std::remove_cv<T>::type, bool>::value ||
  (std::is_convertible<T, bool>::value && !std::is_arithmetic<T>::value &&
   !std::is_enum<T>::value)
> {};

template<typename T>
struct is_boolish<T&> : is_boolish<T> {};

template<typename T>
struct is_boolish<T&&> : is_boolish<T> {};

template<typename T>
struct is_any_char_helper : std::false_type {};

template<> struct is_any_char_helper<char> : std::true_type {};
template<> struct is_any_char_helper<signed char> : std::true_type {};
template<> struct is_any_char_helper<unsigned char> : std::true_type {};

template<> struct is_any_char_helper<wchar_t> : std::true_type {};
template<> struct is_any_char_helper<char16_t> : std::true_type {};
template<> struct is_any_char_helper<char32_t> : std::true_type {};

template<typename T>
struct is_any_char : is_any_char_helper<typename std::remove_cv<T>::type> {};

template<typename T>
class is_iterable {
  template<typename U> struct always_bool { typedef bool type; };

  template<typename U>
  static constexpr typename always_bool<
    decltype( std::begin(std::declval<U>()), std::end(std::declval<U>()) )
  >::type check_(int) {
    return true;
  }
  template<typename U>
  static constexpr bool check_(...) {
    return false;
  }
public:
  static const bool value = check_<T>(0);
};

template<typename T, size_t N>
class is_iterable<T[N]> : public std::true_type {};

// The ensure_printable overloads below are rather complicated, to say the
// least; we need to be extra-careful to ensure that things which are
// convertible to bool don't erroneously get printed out *as* bools. As such, if
// a type is "bool-ish" (convertible to bool, but not a non-bool arithmetic
// type), we delegate to ensure_printable_boolish. Here's the basic structure:
//
// ensure_printable:
//   if is_printable
//     if is_boolish -> ensure_printable_boolish
//     else -> pass-through
//   else
//     if is_iterable -> iterable
//     else -> fallback
//
// ensure_printable_boolish:
//   if is_bool -> bool
//   if is_pointer
//     if is_any_char -> c string
//     else if is_function -> function pointer
//     else -> pointer
//   if !is_scalar -> fallback
//
// We also have an overload for array types, which is selected if it's *not* an
// array of char (arrays of char ultimately get selected by a const char *
// overload). Finally, we have a few non-generic overloads for nullptrs,
// std::strings, and std::pairs/std::tuples.

// Non-generic overloads

inline std::string ensure_printable(std::nullptr_t) {
  return "nullptr";
}

inline auto ensure_printable(const std::string &s) {
  return std::quoted(s);
}

inline auto ensure_printable(const std::wstring &s) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
  return detail::quoted_temp(conv.to_bytes(s));
}

inline auto ensure_printable(const std::u16string &s) {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
  return detail::quoted_temp(conv.to_bytes(s));
}

inline auto ensure_printable(const std::u32string &s) {
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
  return detail::quoted_temp(conv.to_bytes(s));
}

inline auto ensure_printable(char c) {
  return detail::quoted_temp(std::string(1, c), '\'');
}

inline auto ensure_printable(unsigned char c) {
  return detail::quoted_temp(std::string(1, c), '\'');
}

inline auto ensure_printable(signed char c) {
  return detail::quoted_temp(std::string(1, c), '\'');
}

inline auto ensure_printable(wchar_t c) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
  return detail::quoted_temp(conv.to_bytes(std::wstring(1, c)), '\'');
}

inline auto ensure_printable(char16_t c) {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
  return detail::quoted_temp(conv.to_bytes(std::u16string(1, c)), '\'');
}

inline auto ensure_printable(char32_t c) {
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
  return detail::quoted_temp(conv.to_bytes(std::u32string(1, c)), '\'');
}

// Helper for bool-ish types

template<typename T>
inline auto ensure_printable_boolish(T b) -> typename std::enable_if<
  std::is_same<typename std::remove_cv<T>::type, bool>::value, std::string
>::type {
  return b ? "true" : "false";
}

template<typename T>
constexpr inline auto ensure_printable_boolish(const T *t) ->
typename std::enable_if<
  !std::is_function<T>::value, const T *
>::type {
  return t;
}

template<typename Ret, typename ...Args>
inline auto ensure_printable_boolish(Ret (*)(Args...)) {
  return type_name<Ret(Args...)>();
}

// XXX: These don't work for volatile strings.

inline auto ensure_printable_boolish(const char *s) {
  return std::quoted(s);
}

inline auto ensure_printable_boolish(const unsigned char *s) {
  return std::quoted(reinterpret_cast<const char*>(s));
}

inline auto ensure_printable_boolish(const signed char *s) {
  return std::quoted(reinterpret_cast<const char*>(s));
}

inline auto ensure_printable_boolish(const wchar_t *s) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
  return detail::quoted_temp(conv.to_bytes(s));
}

inline auto ensure_printable_boolish(const char16_t *s) {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
  return detail::quoted_temp(conv.to_bytes(s));
}

inline auto ensure_printable_boolish(const char32_t *s) {
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
  return detail::quoted_temp(conv.to_bytes(s));
}

template<typename T>
auto ensure_printable_boolish(const T &t) -> typename std::enable_if<
  !std::is_scalar<typename std::remove_reference<T>::type>::value, std::string
>::type {
  try {
    return typeid(t).name();
  }
  catch(...) {
    return "...";
  }
}

// Pass-through

template<typename T>
constexpr auto ensure_printable(const T &t) -> typename std::enable_if<
  is_printable<T>::value && !is_boolish<T>::value, T
>::type {
  return t;
}

// Bool-ish types

template<typename T>
constexpr inline auto ensure_printable(const T &t) -> typename std::enable_if<
  is_printable<T>::value && is_boolish<T>::value,
  decltype(ensure_printable_boolish(t))
>::type {
  return ensure_printable_boolish(t);
}

// Fallback

template<typename T>
auto ensure_printable(const T &t) -> typename std::enable_if<
  !is_printable<T>::value && !is_iterable<T>::value,
  std::string
>::type {
  try {
    return typeid(t).name();
  }
  catch(...) {
    return "...";
  }
}

// Iterables

template<typename T>
auto ensure_printable(const T &v) -> typename std::enable_if<
  !is_printable<T>::value && is_iterable<T>::value, std::string
>::type {
  return detail::stringify_iterable(std::begin(v), std::end(v));
}

template<typename T, size_t N>
auto ensure_printable(const T (&v)[N]) -> typename std::enable_if<
  !is_any_char<T>::value, std::string
>::type {
  return detail::stringify_iterable(std::begin(v), std::end(v));
}

// Pairs/Tuples

template<typename T, typename U>
std::string ensure_printable(const std::pair<T, U> &pair) {
  return detail::stringify_tuple(pair);
}

template<typename ...T>
std::string ensure_printable(const std::tuple<T...> &tuple) {
  return detail::stringify_tuple(tuple);
}

namespace detail {
  template<typename T>
  std::string stringify_iterable(const T &begin, const T &end) {
    std::stringstream s;
    s << "[";
    if(begin != end) {
      auto i = begin;
      s << ensure_printable(*i);
      for(++i; i != end; ++i)
        s << ", " << ensure_printable(*i);
    }
    s << "]";
    return s.str();
  }

  template<typename T>
  std::string stringify_tuple(const T &tuple) {
    std::stringstream s;
    s << "[";
    reduce_tuple(tuple, [&s](bool first, const auto &x, bool &) {
      if(!first)
        s << ", ";
      s << ensure_printable(x);
      return false;
    }, true);
    s << "]";
    return s.str();
  }
}

} // namespace mettle

#endif
