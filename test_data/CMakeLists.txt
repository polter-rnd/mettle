file(GLOB_RECURSE TEST_DATA_SRC *.cpp)

add_custom_target(test_data)

foreach(TEST_DATA ${TEST_DATA_SRC})
    get_filename_component(TEST_DATA_TARGET ${TEST_DATA} NAME_WE)
    add_executable(${TEST_DATA_TARGET} 
        ${TEST_DATA}
    )
    target_link_libraries(${TEST_DATA_TARGET} libmettle)
    add_dependencies(test_data ${TEST_DATA_TARGET})
endforeach()
