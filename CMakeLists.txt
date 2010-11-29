cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(cportage C)

# Increase when incompatible API change happens, reset MINOR and PATCH.
set(CP_VERSION_MAJOR 0)
# Increase when compatible API change happens, reset PATCH
set(CP_VERSION_MINOR 0)
# Increase when implementation changes
set(CP_VERSION_PATCH 1)

set(CP_VERSION "${CP_VERSION_MAJOR}.${CP_VERSION_MINOR}.${CP_VERSION_PATCH}")

# Compiler flags
set(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
    -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=500 -DCPORTAGE_COMPILATION
    -DG_DISABLE_DEPRECATED -DG_DISABLE_SINGLE_INCLUDES)
set(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
    -pedantic -ansi -fvisibility=hidden -fmessage-length=0 -finput-charset=UTF-8
    -fdiagnostics-show-option -ffast-math -O -Wabi
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
include(FindPkgConfig)
set(GLIB_MINIMAL_REQUIRED 2.20)

pkg_check_modules(GLIB2 REQUIRED "glib-2.0>=${GLIB_MINIMAL_REQUIRED}")
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
include_directories(SYSTEM ${GLIB2_INCLUDE_DIRS})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Platform checks
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCSourceRuns)

check_symbol_exists(realpath stdlib.h HAVE_REALPATH)
if(NOT HAVE_REALPATH)
    message(FATAL_ERROR "Your system doesn't have realpath() function")
endif()

check_c_source_runs(
    "#include <stdlib.h>\n int main(void) {return realpath(\".\", NULL) == NULL;}"
    REALPATH_ACCEPTS_NULL)
if(NOT REALPATH_ACCEPTS_NULL)
    message(FATAL_ERROR "Your realpath() function doesn't accept NULL")
endif()

check_symbol_exists(uname sys/utsname.h HAVE_UNAME)
if(NOT HAVE_UNAME)
    message(FATAL_ERROR "Your system doesn't have uname() function")
endif()

check_symbol_exists(setlocale locale.h HAVE_SETLOCALE)
if(NOT HAVE_SETLOCALE)
    message(FATAL_ERROR "Your system doesn't have setlocale() function")
endif()

check_symbol_exists(qsort stdlib.h HAVE_QSORT)
if(NOT HAVE_QSORT)
    message(FATAL_ERROR "Your system doesn't have qsort() function")
endif()

check_include_file(unistd.h HAVE_UNISTD_H)
if(HAVE_UNISTD_H)
    check_symbol_exists(nice unistd.h HAVE_NICE)
    check_symbol_exists(getpid unistd.h HAVE_GETPID)
    check_symbol_exists(read unistd.h HAVE_READ)
    check_symbol_exists(execlp unistd.h HAVE_EXECLP)
endif()

if(NOT HAVE_EXECLP)
    message(FATAL_ERROR "Your system doesn't have execlp() function")
endif()

check_include_file(sys/resource.h HAVE_RESOURCE_H)
if(HAVE_RESOURCE_H)
    check_symbol_exists(getpriority sys/resource.h HAVE_GETPRIORITY)
    check_symbol_exists(setpriority sys/resource.h HAVE_SETPRIORITY)
endif()

# Macros
macro(set_link_flags _target)
  set_target_properties(${_target} PROPERTIES
      LINK_FLAGS "-Wl,--as-needed,--no-add-needed,--fatal-warnings,--no-undefined,--warn-shared-textrel,-z=now,-z=noexecstack,--no-undefined-version")
endmacro()

# Subdirs
add_subdirectory(man)
add_subdirectory(include)
add_subdirectory(src)

option(ENABLE_TESTS "Enable unit tests" ON)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
