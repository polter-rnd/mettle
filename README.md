# mettle

[![Latest release][release-image]][release-link]
[![Documentation][documentation-image]][documentation-link]
[![Build status][ci-image]][ci-link]

**mettle** is a "batteries included" unit testing framework for C++20. Its
mission is to provide a full toolbox to address your testing needs and to look
good doing it.

## Features

#### Build your own assertions

Expectations (assertions) are defined using composable matchers that
automatically generate human-readable output, ensuring even complex objects are
easy to test.

#### Nest your tests

Suites group your tests together and can be nested as deeply as you need,
so you can use their hierarchy to set up and tear down your fixtures for you.

#### Don't repeat yourself

Type- and value-parameterized tests let you write your tests once and apply them
to multiple implementations or preconditions.

#### Aggregate everything

The `mettle` unified test runner makes it a snap to write multiple, independent
test files – even ones running completely different kinds of tests – and
aggregate them into a single list of results.

## A Brief Example

A picture is worth a thousand words, and code's almost as good (I'm sure it's
worth at least 100 words), so let's take a look at a test file:

```c++
#include <mettle.hpp>
using namespace mettle;

suite<> basic("a basic suite", [](auto &_) {

  _.test("a test", []() {
    expect(true, equal_to(true));
  });

  for(int i = 0; i < 4; i++) {
    _.test("test number " + std::to_string(i), [i]() {
      expect(i % 2, less(2));
    });
  }

  subsuite<>(_, "a subsuite", [](auto &_) {
    _.test("a sub-test", []() {
      expect(true, equal_to(true));
    });
  });

});
```

## Building mettle with CMake

Available options:
- `METTLE_BUILD_EXECUTABLE`: Build mettle executable (default: ON if top-level).
- `METTLE_BUILD_EXAMPLES`: Build test examples (default: OFF).
- `METTLE_SAFE_EXIT`: Use safe exit function (_exit) when exiting test subprocesses (default: ON).

## Using mettle in your project

You can use `mettle` in your CMake project either by using `find_package` or `FetchContent`.

### Using find_package

First, install `mettle` to your system or specify the path to the installation directory.
Then, you can use `find_package` to locate `mettle` and link it to your target:

```cmake
cmake_minimum_required(VERSION 3.12)
project(MyProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(mettle REQUIRED)

add_executable(my_tests test.cpp)
target_link_libraries(my_tests PRIVATE mettle::mettle)

enable_testing()
add_test(NAME my_tests COMMAND my_tests)
```

### Using FetchContent

If you prefer to include `mettle` directly from its repository, you can use `FetchContent`:

```cmake
cmake_minimum_required(VERSION 3.12)
project(MyProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(mettle
  GIT_REPOSITORY https://github.com/jimporter/mettle.git
)
FetchContent_MakeAvailable(mettle)

add_executable(my_tests test.cpp)
target_link_libraries(my_tests PRIVATE mettle::mettle)

enable_testing()
add_test(NAME my_tests COMMAND my_tests)
```

## License

This library is licensed under the [BSD 3-Clause license](LICENSE).

[release-image]: https://img.shields.io/github/release/jimporter/mettle.svg
[release-link]: https://github.com/jimporter/mettle/releases/latest
[documentation-image]: https://img.shields.io/badge/docs-mettle-blue.svg
[documentation-link]: https://jimporter.github.io/mettle/
[ci-image]: https://github.com/jimporter/mettle/actions/workflows/build.yml/badge.svg
[ci-link]: https://github.com/jimporter/mettle/actions/workflows/build.yml?query=branch%3Amaster
