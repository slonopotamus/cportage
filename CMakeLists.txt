cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(cportage C)
set(cportage_VERSION 0.1-git)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Dependencies
find_package(GLIB2 REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Compiler flags
set(CMAKE_REQUIRED_FLAGS -pedantic -ansi
    -fvisibility=hidden -fstack-protector-all -fstrict-aliasing -fdiagnostics-show-option
    -Wall -Wextra -Werror -Wno-import -Wformat=2 -Wmissing-include-dirs
    -Wswitch-default -Wswitch-enum -Wfloat-equal
    -Wdeclaration-after-statement -Wundef -Wshadow
    -Wunsafe-loop-optimizations -Wbad-function-cast -Wcast-align
    -Wwrite-strings -Wlogical-op -Wstrict-prototypes
    -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations
    -Wmissing-noreturn -Wmissing-format-attribute -Wpacked
    -Wredundant-decls -Wnested-externs -Wunreachable-code -Winline
    -Wvolatile-register-var -Wstack-protector
)
# -Wconversion
set(CMAKE_REQUIRED_DEFINITIONS -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=700
    -DG_DISABLE_DEPRECATED -DG_DISABLE_SINGLE_INCLUDES
)

include_directories(${GLIB2_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_definitions(${CMAKE_REQUIRED_FLAGS} ${CMAKE_REQUIRED_DEFINITIONS})

# Platform checks
include(CheckIncludeFile)
include(CheckSymbolExists)

check_symbol_exists(realpath stdlib.h HAVE_REALPATH)
if(NOT HAVE_REALPATH)
    message(FATAL_ERROR "Your system doesn't have realpath function")
endif()

check_symbol_exists(uname sys/utsname.h HAVE_UNAME)
if(NOT HAVE_UNAME)
    message(FATAL_ERROR "Your system doesn't have uname function")
endif()

check_include_file(stdbool.h HAVE_STDBOOL_H)
if(NOT HAVE_STDBOOL_H)
    message(FATAL_ERROR "Your system doesn't have stdbool.h")
endif()

check_symbol_exists(setlocale locale.h HAVE_SETLOCALE)
if(NOT HAVE_SETLOCALE)
    message(FATAL_ERROR "Your system doesn't have setlocale function")
endif()

# Subdirs
add_subdirectory(include)
add_subdirectory(src)

option(WITH_TESTS "Enable unit tests" ON)
if(WITH_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
