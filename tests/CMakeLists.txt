add_executable(tests test.cpp)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/pixello/src)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(tests SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/doctest)
target_include_directories(tests SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/scope_guard)

target_link_libraries(tests PRIVATE pixello)
add_test(NAME tests COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests)
