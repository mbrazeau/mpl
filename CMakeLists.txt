cmake_minimum_required (VERSION 3.1)
project(mpl)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory (src)
#if (NOT MSVC)
add_subdirectory (Tests)
#endif()


