# - Find popt
# Find the native popt includes and library
#
#  Popt_INCLUDE_DIR - where to find popt.h, etc.
#  Popt_LIBRARIES   - List of libraries when using popt.
#  Popt_FOUND       - True if popt found.


IF (Popt_INCLUDE_DIR)
  # Already in cache, be silent
  SET(Popt_FIND_QUIETLY TRUE)
ENDIF (Popt_INCLUDE_DIR)

FIND_PATH(Popt_INCLUDE_DIR popt.h)

FIND_LIBRARY(Popt_LIBRARY NAMES popt )

# handle the QUIETLY and REQUIRED arguments and set Popt_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Popt DEFAULT_MSG Popt_LIBRARY Popt_INCLUDE_DIR)

IF(Popt_FOUND)
  SET( Popt_LIBRARIES ${Popt_LIBRARY} )
ELSE(Popt_FOUND)
  SET( Popt_LIBRARIES )
ENDIF(Popt_FOUND)

MARK_AS_ADVANCED( Popt_LIBRARY Popt_INCLUDE_DIR )
