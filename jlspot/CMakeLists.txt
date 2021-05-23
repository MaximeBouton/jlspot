project(jlspot)

cmake_minimum_required(VERSION 3.1)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

list(APPEND CMAKE_PREFIX_PATH "/home/maxime/cxxwrap-test/libcxxwrap-julia-build")

find_package(JlCxx)
get_target_property(JlCxx_location JlCxx::cxxwrap_julia LOCATION)
get_filename_component(JlCxx_location ${JlCxx_location} DIRECTORY)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${JlCxx_location}")

add_library(jlspot SHARED jlspot.cpp)

target_include_directories(jlspot PUBLIC "/home/maxime/spot-2.9.6/", "/home/maxime/spot-2.9.6/buddy/src")

target_link_libraries(jlspot JlCxx::cxxwrap_julia JlCxx::cxxwrap_julia_stl)

install(TARGETS jlspot
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
