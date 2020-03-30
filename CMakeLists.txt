cmake_minimum_required (VERSION 3.1)
#delete this when integrated to larger project
project(mpl)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory (src)
#if (NOT MSVC)
add_subdirectory (Tests)
#endif()


