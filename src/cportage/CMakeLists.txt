bison_target(ShellParser
    shellparser.y
    "${CMAKE_CURRENT_BINARY_DIR}/shellparser.c"
    COMPILE_FLAGS "-Wall -Werror -r all")
flex_target(ShellScanner
    shellscanner.l
    "${CMAKE_CURRENT_BINARY_DIR}/shellscanner.c"
    COMPILE_FLAGS "--header-file=${CMAKE_CURRENT_BINARY_DIR}/shellscanner.h")
add_flex_bison_dependency(ShellScanner ShellParser)

add_definitions(-DG_LOG_DOMAIN="cportage" -DGETTEXT_PACKAGE="cportage")

configure_file(cportage.pc.in "${CMAKE_CURRENT_BINARY_DIR}/cportage.pc")

# Executables
file(GLOB cportage_sources *.c)
add_library(cportage SHARED
    ${cportage_sources}
    ${BISON_ShellParser_OUTPUTS}
    ${FLEX_ShellScanner_OUTPUTS})
target_link_libraries(cportage ${GLIB2_LIBRARIES})
set_target_properties(cportage PROPERTIES
  SOVERSION "${CP_VERSION_MAJOR}"
  VERSION "${CP_VERSION}")
set_link_flags(cportage)

# Installation
install(TARGETS cportage LIBRARY DESTINATION lib)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cportage.pc" DESTINATION lib/pkgconfig)
