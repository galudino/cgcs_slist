cmake_minimum_required(VERSION "3.18")
project("cgcs_slist_demo")

set(C_STANDARD "11")
set(CFLAGS "-Wall -Werror -pedantic-errors")

set(CMAKE_C_STANDARD ${C_STANDARD})
set(CMAKE_C_FLAGS ${CFLAGS})

add_executable("cgcs_slist_demo" "cgcs_slist_demo.c")
add_compile_options("cgcs_slist_demo" PUBLIC "-fblocks")
target_link_libraries("cgcs_slist_demo" LINK_PUBLIC "cgcs_slist")
