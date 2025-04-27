cmake_minimum_required(VERSION 3.8)

project(lab2 LANGUAGES CXX)

set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_CXX_FLAGS_DEBUG   "-Wall -Wextra -g -O0 -D DEBUG -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -DNDEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(bitonic_sort ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(bitonic_sort PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(bitonic_sort PRIVATE pthread)

find_package(GTest REQUIRED)
enable_testing()

add_executable(sanity_test ${CMAKE_CURRENT_SOURCE_DIR}/test/googletest/sanity_test.cpp)
target_include_directories(sanity_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(sanity_test PRIVATE pthread)
target_link_libraries(sanity_test PRIVATE ${GTEST_LIBRARIES})

add_executable(generative_test ${CMAKE_CURRENT_SOURCE_DIR}/test/googletest/generative_test.cpp)
target_include_directories(generative_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(generative_test PRIVATE pthread)
target_link_libraries(generative_test PRIVATE ${GTEST_LIBRARIES})
