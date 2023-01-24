add_executable(tests test.cpp)

target_include_directories(tests SYSTEM PRIVATE ../src)
target_include_directories(tests SYSTEM PRIVATE ../doctest)

target_link_libraries(tests PRIVATE pixello)
add_test(NAME tests COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests)
