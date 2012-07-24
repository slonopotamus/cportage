if(HAVE_READ AND HAVE_FILENO)
    set(flex_mode "--fast")
else()
    set(flex_mode "--nounistd")
endif()

macro(add_flex_bison_target _name)
  flex_target("${_name}_scanner"
    "${_name}_scanner.l"
    "${CMAKE_CURRENT_BINARY_DIR}/${_name}_scanner.c"
    COMPILE_FLAGS
    "${flex_mode} --header-file=${CMAKE_CURRENT_BINARY_DIR}/${_name}_scanner.h")
  bison_target("${_name}_parser"
    "${_name}_parser.y"
    "${CMAKE_CURRENT_BINARY_DIR}/${_name}_parser.c"
    COMPILE_FLAGS "-Wall -Werror --report=all --debug")
  add_flex_bison_dependency("${_name}_scanner" "${_name}_parser")
endmacro()

add_flex_bison_target("atom")
add_flex_bison_target("shell")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" sources)
set(sources
  ${sources}
  ${BISON_atom_parser_OUTPUTS} ${FLEX_atom_scanner_OUTPUTS}
  ${BISON_shell_parser_OUTPUTS} ${FLEX_shell_scanner_OUTPUTS}
)

add_library(cportage SHARED ${sources})
target_link_libraries(cportage ${GLIB2_LIBRARIES} ${GMP_LIBRARY})
set_target_properties(cportage PROPERTIES
  SOVERSION "${CP_VERSION_MAJOR}"
  VERSION "${CP_VERSION}")
set_link_flags(cportage)

add_library(cportage_static STATIC EXCLUDE_FROM_ALL ${sources})
set_link_flags(cportage_static)

configure_file(cportage.pc.in "${CMAKE_CURRENT_BINARY_DIR}/cportage.pc")

install(TARGETS cportage LIBRARY DESTINATION lib)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cportage.pc" DESTINATION lib/pkgconfig)
