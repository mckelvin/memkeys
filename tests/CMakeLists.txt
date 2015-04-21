include_directories(${GTEST_INCLUDE_DIRS})
file(GLOB TEST_SRC_FILES ${PROJECT_SOURCE_DIR}/tests/test_*.cpp)

foreach(SRC ${TEST_SRC_FILES})
    get_filename_component(test_name ${SRC} NAME_WE)
    set_source_files_properties(${SRC} PROPERTIES COMPILE_FLAGS "-fexceptions")
    add_executable(${test_name} ${SRC})
    add_dependencies(${test_name} gtest)
    target_link_libraries(${test_name}
        ${GTEST_LIBS_DIR}/libgtest.a
        ${GTEST_LIBS_DIR}/libgtest_main.a
        memkeys-core
    )
    add_test(${test_name} ${test_name})
endforeach(SRC)
