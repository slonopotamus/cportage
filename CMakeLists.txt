cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(cportage C)

# Increase when incompatible API change happens, reset MINOR and PATCH.
set(CP_VERSION_MAJOR 0)
# Increase when compatible API change happens, reset PATCH
set(CP_VERSION_MINOR 0)
# Increase when implementation changes
set(CP_VERSION_PATCH 1)

set(CP_VERSION "${CP_VERSION_MAJOR}.${CP_VERSION_MINOR}.${CP_VERSION_PATCH}")

##################
# Compiler flags #
##################

set(CMAKE_REQUIRED_DEFINITIONS
    -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=700 -D_FILE_OFFSET_BITS=64
    -DCPORTAGE_COMPILATION -DG_LOG_DOMAIN="${CMAKE_PROJECT_NAME}"
    -DGETTEXT_PACKAGE="${CMAKE_PROJECT_NAME}"
    -DG_DISABLE_DEPRECATED -DG_DISABLE_SINGLE_INCLUDES)

set(CMAKE_REQUIRED_FLAGS
    -std=c89 -pedantic -fvisibility=hidden -fmessage-length=0 -finput-charset=UTF-8
    -fdiagnostics-show-option -Wabi
    -Wall -Wextra -Werror -Wformat=2 -Winit-self -Wunknown-pragmas
    -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wunused -ftree-vrp
    -fstrict-overflow -Wstrict-overflow=5 -Wfloat-equal -Wsync-nand
    -Wdeclaration-after-statement -Wundef -Wshadow -Wno-overlength-strings
    -Wbad-function-cast -Wcast-qual -Wcast-align -Wpacked-bitfield-compat
    -Wwrite-strings -Wconversion -Wlogical-op -Waggregate-return -Wvla
    -Wmissing-noreturn -Wmissing-format-attribute -Wnormalized=nfkc -Wpacked
    -Wpadded -Wno-error=padded -Wredundant-decls -Wnested-externs -Winline
    -Winvalid-pch -Wvolatile-register-var -Wdisabled-optimization -Wimport
    -Wstack-protector -fstack-protector -fstack-protector-all -fstrict-aliasing)

add_definitions(${CMAKE_REQUIRED_DEFINITIONS} ${CMAKE_REQUIRED_FLAGS}
    -Wold-style-definition -Wmissing-prototypes
    -Wmissing-declarations -Wstrict-prototypes)

################
# Dependencies #
################

include(FindPkgConfig)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckCSourceRuns)

set(GLIB_MINIMAL_REQUIRED 2.28)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

find_library(GMP_LIBRARY gmp)
if(NOT GMP_LIBRARY)
    message(FATAL_ERROR "GMP library not found")
endif()
check_include_file(gmp.h HAVE_GMP_H)
if(NOT HAVE_GMP_H)
    message(FATAL_ERROR "<gmp.h> header not found")
endif()

pkg_check_modules(GLIB2 REQUIRED "glib-2.0 >= ${GLIB_MINIMAL_REQUIRED}")

set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
include_directories(SYSTEM ${GLIB2_INCLUDE_DIRS})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

###################
# Platform checks #
###################

check_symbol_exists(realpath stdlib.h HAVE_REALPATH)
if(NOT HAVE_REALPATH)
    message(FATAL_ERROR "Your system doesn't have realpath() function")
endif()

check_symbol_exists(fileno stdio.h HAVE_FILENO)

check_c_source_runs(
    "#include <stdlib.h>\n int main(void) {return realpath(\".\", NULL) == NULL;}"
    REALPATH_ACCEPTS_NULL)
if(NOT REALPATH_ACCEPTS_NULL)
    message(FATAL_ERROR "Your realpath() function doesn't accept NULL")
endif()

check_include_file(sys/utsname.h HAVE_UTSNAME_H)
if(HAVE_UTSNAME_H)
    check_symbol_exists(uname sys/utsname.h HAVE_UNAME)
endif()

check_include_file(locale.h HAVE_LOCALE_H)
if(HAVE_LOCALE_H)
    check_symbol_exists(setlocale locale.h HAVE_SETLOCALE)
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

check_include_file(sys/resource.h HAVE_RESOURCE_H)
if(HAVE_RESOURCE_H)
    check_symbol_exists(getpriority sys/resource.h HAVE_GETPRIORITY)
    check_symbol_exists(setpriority sys/resource.h HAVE_SETPRIORITY)
endif()

configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

##########
# Macros #
##########

macro(set_link_flags _target)
  set_target_properties(${_target} PROPERTIES
      LINK_FLAGS "-Wl,--as-needed,--no-add-needed,--fatal-warnings,--no-undefined,--warn-shared-textrel,-z=now,-z=noexecstack,--no-undefined-version")
endmacro()

###########
# Subdirs #
###########

add_subdirectory(include)
add_subdirectory(man)
add_subdirectory(src)

option(ENABLE_TESTS "Enable unit tests" ON)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

option(ENABLE_NLS "Enable Native Language Support" ON)
if(ENABLE_NLS)
    add_subdirectory(po)
endif()
