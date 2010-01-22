# - Find popt
# Find the native popt includes and library
#
#  POPT_INCLUDE_DIR - where to find popt.h, etc.
#  POPT_LIBRARIES   - List of libraries when using popt.
#  POPT_FOUND       - True if popt found.

if(POPT_INCLUDE_DIR)
  # Already in cache, be silent
  set(POPT_FIND_QUIETLY TRUE)
endif()

find_path(POPT_INCLUDE_DIR popt.h)

find_library(POPT_LIBRARY NAMES popt)

# handle the QUIETLY and REQUIRED arguments and set POPT_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(POPT DEFAULT_MSG POPT_LIBRARY POPT_INCLUDE_DIR)

if(POPT_FOUND)
  set(POPT_LIBRARIES ${POPT_LIBRARY})
else()
  set(POPT_LIBRARIES)
endif()

mark_as_advanced(POPT_LIBRARY POPT_INCLUDE_DIR)
