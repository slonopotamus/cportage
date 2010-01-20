# - Find PCRE
# Find the native PCRE includes and library
#
#  PCRE_INCLUDE_DIR - where to find pcre.h, etc.
#  PCRE_LIBRARIES   - List of libraries when using pcre.
#  PCRE_FOUND       - True if pcre found.

if(PCRE_INCLUDE_DIR)
  # Already in cache, be silent
  set(PCRE_FIND_QUIETLY TRUE)
endif()

find_path(PCRE_INCLUDE_DIR pcre.h)

find_library(PCRE_LIBRARY NAMES pcre)

# handle the QUIETLY and REQUIRED arguments and set PCRE_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PCRE DEFAULT_MSG PCRE_LIBRARY PCRE_INCLUDE_DIR)

if(PCRE_FOUND)
  set(PCRE_LIBRARIES ${PCRE_LIBRARY})
else()
  set(PCRE_LIBRARIES)
endif()

mark_as_advanced(PCRE_LIBRARY PCRE_INCLUDE_DIR)
