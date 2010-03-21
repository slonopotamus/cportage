cmake_minimum_required(VERSION 2.6)
project(cportage C)
set(cportage_VERSION 0.1-git)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Dependencies
find_package(GLIB2 REQUIRED)
#find_package(BISON REQUIRED)
#find_package(FLEX REQUIRED)

set(CMAKE_REQUIRED_FLAGS -pedantic -ansi
  -fvisibility=hidden -fstack-protector
  -Wall -Wextra -Werror -Wwrite-strings -Wstrict-prototypes -Wstack-protector
  -Wmissing-prototypes -Wold-style-definition -Wmissing-format-attribute)
set(CMAKE_REQUIRED_DEFINITIONS -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=700
  -DG_DISABLE_DEPRECATED -DG_DISABLE_SINGLE_INCLUDES -DG_LOG_DOMAIN="cportage"
)

include_directories(${GLIB2_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_definitions(${CMAKE_REQUIRED_FLAGS} ${CMAKE_REQUIRED_DEFINITIONS})

add_subdirectory(include)
add_subdirectory(src)

option(WITH_TESTS "Enable unit tests" ON)
if(WITH_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
