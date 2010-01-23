# - Find libhash
# Find the native libhash includes and library
#
#  Libhash_INCLUDE_DIR - where to find libhash.h, etc.
#  Libhash_LIBRARIES   - List of libraries when using libhash.
#  Libhash_FOUND       - True if libhash found.

if(Libhash_INCLUDE_DIR)
  # Already in cache, be silent
  set(Libhash_FIND_QUIETLY TRUE)
endif()

find_path(Libhash_INCLUDE_DIR hash.h)

find_library(Libhash_LIBRARY NAMES hash)

# handle the QUIETLY and REQUIRED arguments and set Libhash_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libhash DEFAULT_MSG Libhash_LIBRARY Libhash_INCLUDE_DIR)

if(Libhash_FOUND)
  set(Libhash_LIBRARIES ${Libhash_LIBRARY})
else()
  set(Libhash_LIBRARIES)
endif()

mark_as_advanced(Libhash_LIBRARY Libhash_INCLUDE_DIR)
