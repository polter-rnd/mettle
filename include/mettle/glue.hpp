#ifndef INC_METTLE_GLUE_HPP
#define INC_METTLE_GLUE_HPP

#include "suite.hpp"
#include "matchers/error.hpp"

namespace mettle {

template<typename ...Fixture, typename Factory, typename F>
inline runnable_suite
make_suite(const std::string &name, const attr_list &attrs,
           Factory &&factory, F &&f) {
  return make_basic_suite<expectation_error, Fixture...>(
    name, attrs,  std::forward<Factory>(factory), std::forward<F>(f)
  );
}

template<typename ...Fixture, typename Factory, typename F>
inline runnable_suite
make_suite(const std::string &name, Factory &&factory, F &&f) {
  return make_basic_suite<expectation_error, Fixture...>(
    name, std::forward<Factory>(factory), std::forward<F>(f)
  );
}

template<typename ...Fixture, typename F>
inline runnable_suite
make_suite(const std::string &name, const attr_list &attrs, F &&f) {
  return make_suite<Fixture...>(name, attrs, auto_factory, std::forward<F>(f));
}

template<typename ...Fixture, typename F>
inline runnable_suite
make_suite(const std::string &name, F &&f) {
  return make_suite<Fixture...>(name, auto_factory, std::forward<F>(f));
}

template<typename ...Fixture, typename Factory, typename F>
inline auto
make_suites(const std::string &name, const attr_list &attrs,
           Factory &&factory, F &&f) {
  return make_basic_suites<expectation_error, Fixture...>(
    name, attrs,  std::forward<Factory>(factory), std::forward<F>(f)
  );
}

template<typename ...Fixture, typename Factory, typename F>
inline auto make_suites(const std::string &name, Factory &&factory, F &&f) {
  return make_basic_suites<expectation_error, Fixture...>(
    name, std::forward<Factory>(factory), std::forward<F>(f)
  );
}

template<typename ...Fixture, typename F>
inline auto
make_suites(const std::string &name, const attr_list &attrs, F &&f) {
  return make_suites<Fixture...>(name, attrs, auto_factory, std::forward<F>(f));
}

template<typename ...Fixture, typename F>
inline auto make_suites(const std::string &name, F &&f) {
  return make_suites<Fixture...>(name, auto_factory, std::forward<F>(f));
}

} // namespace mettle

#endif
