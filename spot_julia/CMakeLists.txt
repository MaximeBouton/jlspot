project(jlspot)

cmake_minimum_required(VERSION 3.1)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

list(APPEND CMAKE_PREFIX_PATH "/home/maxime/cxxwrap-test/libcxxwrap-julia-build")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/../spot-build/lib/")

find_package(JlCxx)
get_target_property(JlCxx_location JlCxx::cxxwrap_julia LOCATION)
get_filename_component(JlCxx_location ${JlCxx_location} DIRECTORY)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${JlCxx_location}")

find_library(SPOT_LIB spot)
find_library(BDD_LIB bddx)

add_library(spot_julia SHARED "spot_julia.cpp")

target_include_directories(spot_julia PUBLIC "${CMAKE_SOURCE_DIR}/../spot-build/include")

target_link_libraries(spot_julia JlCxx::cxxwrap_julia JlCxx::cxxwrap_julia_stl "${SPOT_LIB}" "${BDD_LIB}")

install(TARGETS spot_julia
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
