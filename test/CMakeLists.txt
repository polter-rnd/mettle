file(GLOB_RECURSE TEST_SRC *.cpp)
set(TEST_METTLE_SRC
    ${PROJECT_SOURCE_DIR}/src/mettle/run_test_files.cpp
    ${PROJECT_SOURCE_DIR}/src/mettle/test_command.cpp
)

if(UNIX)
    list(FILTER TEST_SRC EXCLUDE
         REGEX "${CMAKE_CURRENT_SOURCE_DIR}/windows/.*")
    list(APPEND TEST_METTLE_SRC
         ${PROJECT_SOURCE_DIR}/src/mettle/posix/run_test_file.cpp)
elseif(WIN32)
    list(FILTER TEST_SRC EXCLUDE
         REGEX "${CMAKE_CURRENT_SOURCE_DIR}/posix/.*")
    list(APPEND TEST_METTLE_SRC
         ${PROJECT_SOURCE_DIR}/src/mettle/windows/run_test_file.cpp)
endif()

foreach(TEST ${TEST_SRC})
    get_filename_component(TEST_TARGET ${TEST} NAME_WE)

    add_executable(${TEST_TARGET}
        ${TEST}
        ${TEST_METTLE_SRC}
    )
    target_link_libraries(${TEST_TARGET} libmettle)
    target_include_directories(${TEST_TARGET}
        PRIVATE
            ${bencode_SOURCE_DIR}/include
    )
    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})

    if(NOT DEFINED ENV{TEST_DATA})
        set_property(
            TEST ${TEST_TARGET} PROPERTY
            ENVIRONMENT "TEST_DATA=${PROJECT_BINARY_DIR}/test_data"
        )
    endif()
endforeach()
