cmake_minimum_required(VERSION 3.10)

project(quadtree_image_compressor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# compiler warnings and flags
if(MSVC)
  add_compile_options(/W4 /permissive- )
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(/O2)
  endif()
else()
    add_compile_options(-Wall -Wextra -Wno-missing-field-initializers -pedantic -O3)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(quadtree_image_compressor ${SOURCES})

target_include_directories(quadtree_image_compressor PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
)

# find_package(CLI11 REQUIRED)
# target_link_libraries(quadtree_image_compressor PRIVATE CLI11::CLI11)

# if i need
if(EXISTS "${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt")
    enable_testing()
    add_subdirectory(tests)
endif()

