cmake_minimum_required(VERSION 3.12)

# Project configuration
project(mettle VERSION 0.2 LANGUAGES CXX)

# Compiler / language configuration.
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

# Rrequired depencencies
find_package(Boost 1.55 CONFIG REQUIRED COMPONENTS program_options iostreams)
find_package(Threads REQUIRED)

# Download bencode.hpp
include(FetchContent)
fetchcontent_declare(bencode
    GIT_REPOSITORY https://github.com/jimporter/bencode.hpp.git
    GIT_TAG        v1.x
)
fetchcontent_makeavailable(bencode)

# Add an option to control whether the executable is built
option(METTLE_BUILD_EXECUTABLE "Build mettle executable" ${PROJECT_IS_TOP_LEVEL})

# Add an option to control whether examples are built
option(METTLE_BUILD_EXAMPLES "Build test examples" OFF)

# Use safe exit function (_exit) when exiting test subprocesses
option(METTLE_SAFE_EXIT "Use safe exit function (_exit)" ON)

# Add definitions
add_compile_definitions(METTLE_VERSION=${PROJECT_VERSION})
if(METTLE_SAFE_EXIT)
    add_compile_definitions(METTLE_SAFE_EXIT)
endif()

# Library sources
add_subdirectory(src)

# Determine if we should build tests.
include(CMakeDependentOption)
option(BUILD_TESTING "Build unit tests" OFF)
if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
    enable_testing()
    add_subdirectory(test)
    add_subdirectory(test_data)
endif()

if(METTLE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Install man pages and documentation
include(GNUInstallDirs)
install(FILES man/mettle.1
        DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
)
install(FILES README.md CHANGES.md LICENSE
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
)
