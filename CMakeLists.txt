cmake_minimum_required(VERSION 3.16.3)
project(C_STD)
include_directories( include )

if ("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
  add_compile_options(-Wall -Wextra)
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -Werror)
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel")
  MESSAGE("Intel")
elseif ("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  add_compile_options(/W4 /wd4115 /WX)
endif()
set(CMAKE_C_FLAGS, "/std\:clatest")

# Build a static library
add_library( C_STD STATIC src/std_deque.c src/std_item.c src/std_list.c  src/std_vector.c src/std_memory.c src/std_container.c )

# Build a test application using testcode and the static library
add_executable( TestApp testcode/C_STD.c testcode/memory_counter.c) 
target_link_libraries( TestApp C_STD )
