cmake_minimum_required(VERSION 3.8)

project(lab2 LANGUAGES CXX)

set(CMAKE_C_COMPILER "g++")

set(CMAKE_C_FLAGS_DEBUG   "-Wall -Wextra -g -O0 -D DEBUG -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")
set(CMAKE_C_FLAGS_RELEASE "-Wall -O2 -DNDEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(bitonic_sort ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# target_link_libraries(bitonic_sort PRIVATE pthread)
