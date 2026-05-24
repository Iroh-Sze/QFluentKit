cmake_minimum_required(VERSION 3.5)

project(QFluentKit VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_subdirectory(QFluent)
add_subdirectory(QFluentExample)

option(QFLUENT_BUILD_TESTS "Build QFluent tests" OFF)
if(QFLUENT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# find_package(QFluent QUIET)
# if(QFluent_FOUND)
#     add_subdirectory(MinimalExample)
# endif()


