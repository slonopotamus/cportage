cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(cportage C)
set(cportage_VERSION 0.1-git)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Compiler flags
set(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
    -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=500
    -DG_DISABLE_DEPRECATED -DG_DISABLE_SINGLE_INCLUDES)
set(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
    -pedantic -ansi -fvisibility=hidden -fmessage-length=0
    -fdiagnostics-show-option -ffast-math -O -Wabi -Wfatal-errors
    -Wall -Wextra -Werror -Wformat=2 -Winit-self -Wunknown-pragmas
    -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wunused -ftree-vrp
    -fstrict-overflow -Wstrict-overflow=5 -Wfloat-equal -Wsync-nand
    -Wdeclaration-after-statement -Wundef -Wshadow -Wno-overlength-strings
    -Wbad-function-cast -Wcast-qual -Wcast-align
    -Wwrite-strings -Wconversion -Wlogical-op -Waggregate-return -Wvla
    -Wmissing-noreturn -Wmissing-format-attribute -Wnormalized=nfkc -Wpacked
    -Wpadded -Wno-error=padded -Wredundant-decls -Wnested-externs -Winline
    -Winvalid-pch -Wvolatile-register-var -Wdisabled-optimization
    -Wstack-protector -fstack-protector -fstack-protector-all -fstrict-aliasing
    -Wimport -Wdisabled-optimization -Wpacked-bitfield-compat
    -Wunreachable-code -Wno-error=unreachable-code)
add_definitions(${CMAKE_REQUIRED_DEFINITIONS}
    -Wold-style-definition -Wmissing-prototypes
    -Wmissing-declarations -Wstrict-prototypes)

# Dependencies
find_package(GLIB2 REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
include_directories(SYSTEM ${GLIB2_INCLUDE_DIR})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Platform checks
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCSourceRuns)

check_symbol_exists(realpath stdlib.h HAVE_REALPATH)
if(NOT HAVE_REALPATH)
    message(FATAL_ERROR "Your system doesn't have realpath(3) function")
endif()

check_c_source_runs(
    "#include <stdlib.h>\n int main(void) {return realpath(\".\", NULL) == NULL;}"
    REALPATH_ACCEPTS_NULL)
if(NOT REALPATH_ACCEPTS_NULL)
    message(FATAL_ERROR "Your realpath(3) function doesn't accept NULL")
endif()

check_symbol_exists(uname sys/utsname.h HAVE_UNAME)
if(NOT HAVE_UNAME)
    message(FATAL_ERROR "Your system doesn't have uname(2) function")
endif()

check_symbol_exists(setlocale locale.h HAVE_SETLOCALE)
if(NOT HAVE_SETLOCALE)
    message(FATAL_ERROR "Your system doesn't have setlocale(3) function")
endif()

check_symbol_exists(qsort stdlib.h HAVE_QSORT)
if(NOT HAVE_QSORT)
    message(FATAL_ERROR "Your system doesn't have qsort(3) function")
endif()

check_symbol_exists(execlp unistd.h HAVE_EXECLP)
if(NOT HAVE_EXECLP)
    message(FATAL_ERROR "Your system doesn't have execlp(3) function")
endif()

# Subdirs
add_subdirectory(man)
add_subdirectory(include)
add_subdirectory(src)

option(ENABLE_TESTS "Enable unit tests" ON)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
