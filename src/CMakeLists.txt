file(GLOB SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_executable(Reystorm ${SRCS})

target_link_libraries(Reystorm pixello)
target_include_directories(Reystorm PRIVATE ../pixello/src)

# Add the binary as test so we can run it with ctest --verbose
add_test(NAME Reystorm
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/Reystorm
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/..)
