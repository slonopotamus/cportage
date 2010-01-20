cmake_minimum_required(VERSION 2.6)
project(cportage C)
set(cportage_VERSION 0.1-git)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

set(CMAKE_REQUIRED_FLAGS "-std=c99 -pedantic -fvisibility=hidden -fstack-protector"
  "-Wall -Wextra -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Wmissing-format-attribute")
set(CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE -D_FORTIFY_SOURCE=2")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_definitions(${CMAKE_REQUIRED_FLAGS} ${CMAKE_REQUIRED_DEFINITIONS})

add_subdirectory(include)
add_subdirectory(src)

option(WITH_TESTS "Enable unit tests" OFF)

if(WITH_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
