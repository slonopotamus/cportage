if(HAVE_READ AND HAVE_FILENO)
    set(flex_mode "--fast")
else()
    set(flex_mode "--nounistd")
endif()
set(bison_flags "-Wall -Werror --report=all")

macro(add_flex_bison_target _name)
  flex_target("${_name}scanner"
    "${_name}scanner.l"
    "${CMAKE_CURRENT_BINARY_DIR}/${_name}scanner.c"
    COMPILE_FLAGS
    "${flex_mode} --header-file=${CMAKE_CURRENT_BINARY_DIR}/${_name}scanner.h")
  bison_target("${_name}parser"
    "${_name}parser.y"
    "${CMAKE_CURRENT_BINARY_DIR}/${_name}parser.c"
    COMPILE_FLAGS "${bison_flags}")
  add_flex_bison_dependency("${_name}scanner" "${_name}parser")
endmacro()

add_flex_bison_target("shell")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" sources)
set(sources
  ${sources}
  ${BISON_shellparser_OUTPUTS} ${FLEX_shellscanner_OUTPUTS}
)

add_library(cportage SHARED ${sources})
target_link_libraries(cportage ${GLIB2_LIBRARIES})
set_target_properties(cportage PROPERTIES
  SOVERSION "${CP_VERSION_MAJOR}"
  VERSION "${CP_VERSION}")
set_link_flags(cportage)

if(ENABLE_TESTS)
  add_library(cportage_static STATIC ${sources})
  set_link_flags(cportage_static)
endif()

configure_file(cportage.pc.in "${CMAKE_CURRENT_BINARY_DIR}/cportage.pc")

install(TARGETS cportage LIBRARY DESTINATION lib)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cportage.pc" DESTINATION lib/pkgconfig)
