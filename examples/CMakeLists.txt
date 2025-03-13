file(GLOB_RECURSE EXAMPLE_SRC *.cpp)

foreach(EXAMPLE ${EXAMPLE_SRC})
    get_filename_component(EXAMPLE_TARGET ${EXAMPLE} NAME_WE)

    add_executable(${EXAMPLE_TARGET} ${EXAMPLE})
    target_link_libraries(${EXAMPLE_TARGET} libmettle)
endforeach()
