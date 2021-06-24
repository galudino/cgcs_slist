cmake_minimum_required(VERSION "3.18")
project("cgcs_slist")

set(C_STANDARD "11") # C11
set(CFLAGS "-Wall -Werror -pedantic-errors")

set(CMAKE_C_STANDARD "${C_STANDARD}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}")

add_library("cgcs_slist" "cgcs_slist.h" "cgcs_slist.c")
target_include_directories("cgcs_slist" PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
