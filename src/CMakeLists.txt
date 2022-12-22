file(GLOB SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_executable(Raystorm ${SRCS})

target_link_libraries(Raystorm pixello)
target_include_directories(Raystorm PRIVATE ../pixello/src)

# Add the binary as test so we can run it with ctest --verbose
add_test(NAME Raystorm
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/Raystorm
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..)
