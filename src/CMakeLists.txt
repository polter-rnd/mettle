file(GLOB LIBRARY_SRC
    "libmettle/*.cpp"
    "libmettle/log/*.cpp"
)

if(UNIX)
    file(GLOB PLATFORM_SRC
        "libmettle/posix/*.cpp"
        "posix/*.cpp"
        "libmettle/log/posix/*.cpp"
    )
elseif(WIN32)
    file(GLOB PLATFORM_SRC
        "libmettle/windows/*.cpp"
        "windows/*.cpp"
        "libmettle/log/windows/*.cpp"
    )
else()
    message(FATAL_ERROR "Mettle CMakeLists.txt supported only on POSIX or Windows")
endif()

# Library for embedding mettle
add_library(libmettle ${LIBRARY_SRC} ${PLATFORM_SRC})
add_library(mettle::mettle ALIAS libmettle)

# Rename exported target so it's available as mettle::mettle
set_target_properties(libmettle PROPERTIES EXPORT_NAME mettle)
set_target_properties(libmettle PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)

if(UNIX)
    # For POSIX systems library will anyway be named libmettle.so
    set_target_properties(libmettle PROPERTIES OUTPUT_NAME "mettle")
elseif(WIN32)
    if(BUILD_SHARED_LIBS)
        # For Windows we need to export symbols
        target_compile_definitions(libmettle PUBLIC LIBMETTLE_EXPORTS)
    else()
        target_compile_definitions(libmettle PUBLIC METTLE_STATIC)
    endif()
endif()

file(GLOB_RECURSE LIBRARY_HDR
    "../include/*.hpp"
)

target_sources(libmettle
    PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        BASE_DIRS
            ${PROJECT_SOURCE_DIR}/include
        FILES
            ${LIBRARY_HDR}
)

target_include_directories(libmettle
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${bencode_SOURCE_DIR}/include
)

target_link_libraries(libmettle
    PUBLIC
        Boost::program_options Boost::iostreams
)

# Install library and public headers
include(GNUInstallDirs)
install(TARGETS libmettle
        EXPORT mettle-targets
        FILE_SET public_headers
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Save targets to binary dir for use without installing
export(TARGETS libmettle
       NAMESPACE mettle::
       FILE ${PROJECT_BINARY_DIR}/mettle-targets.cmake)

install(EXPORT mettle-targets
        FILE mettle-targets.cmake
        NAMESPACE mettle::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mettle
)

if(METTLE_BUILD_EXECUTABLE)
    if(UNIX)
        file(GLOB EXECUTABLE_SRC
            "mettle/*.cpp"
            "mettle/posix/*.cpp"
        )
    elseif(WIN32)
        file(GLOB EXECUTABLE_SRC
            "mettle/*.cpp"
            "mettle/windows/*.cpp"
        )
    endif()

    # Executable to run tests
    add_executable(mettle ${EXECUTABLE_SRC})

    target_include_directories(mettle
        PRIVATE
            ${bencode_SOURCE_DIR}/include
    )

    target_link_libraries(mettle
        PUBLIC
            libmettle Boost::program_options Boost::iostreams
    )

    # Install binary without exporting
    install(TARGETS mettle
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Install CMake config files
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/mettle-config.cmake.in
    ${PROJECT_BINARY_DIR}/mettle-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mettle
)
write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/mettle-config-version.cmake
    VERSION ${METTLE_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(
    FILES
        ${PROJECT_BINARY_DIR}/mettle-config.cmake
        ${PROJECT_BINARY_DIR}/mettle-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mettle
)

# Install pkg-config file
if(IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
    set(PKG_CONFIG_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
else()
    set(PKG_CONFIG_INCLUDEDIR "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
endif()
if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
    set(PKG_CONFIG_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
else()
    set(PKG_CONFIG_LIBDIR "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
endif()

configure_file(${PROJECT_SOURCE_DIR}/cmake/mettle.pc.in
               ${CMAKE_BINARY_DIR}/mettle.pc
               @ONLY
)
install(FILES ${CMAKE_BINARY_DIR}/mettle.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
