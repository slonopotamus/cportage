# - Find popt
# Find the native popt includes and library
#
#  Popt_INCLUDE_DIR - where to find popt.h, etc.
#  Popt_LIBRARIES   - List of libraries when using popt.
#  Popt_FOUND       - True if popt found.

if(Popt_INCLUDE_DIR)
  # Already in cache, be silent
  set(Popt_FIND_QUIETLY TRUE)
endif()

find_path(Popt_INCLUDE_DIR popt.h)

find_library(Popt_LIBRARY NAMES popt)

# handle the QUIETLY and REQUIRED arguments and set Popt_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Popt DEFAULT_MSG Popt_LIBRARY Popt_INCLUDE_DIR)

if(Popt_FOUND)
  set(Popt_LIBRARIES ${Popt_LIBRARY})
else()
  set(Popt_LIBRARIES)
endif()

mark_as_advanced(Popt_LIBRARY Popt_INCLUDE_DIR)
