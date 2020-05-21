# CMakeList.txt : CMake project for cmake_maker, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("cmake_maker")

# Add source to this project's executable.
add_executable (cmake_maker "cmake_maker.cpp" "cmake_maker.h")
target_compile_features(cmake_maker PUBLIC cxx_std_20)

# TODO: Add tests and install targets if needed.
