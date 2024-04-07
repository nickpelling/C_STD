# CMakeLists.txt

# Copyright (c) 2024 Nick Pelling
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

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

# Enable tests and add test invocations
enable_testing()
add_test(NAME vector_test  COMMAND $<TARGET_FILE:TestApp> vector)
add_test(NAME list_test    COMMAND $<TARGET_FILE:TestApp> list)
add_test(NAME deque_test   COMMAND $<TARGET_FILE:TestApp> deque)
add_test(NAME queue_test   COMMAND $<TARGET_FILE:TestApp> queue)
add_test(NAME stack_test   COMMAND $<TARGET_FILE:TestApp> stack)
add_test(NAME nested_test  COMMAND $<TARGET_FILE:TestApp> nested)
