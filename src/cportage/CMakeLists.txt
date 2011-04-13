bison_target(ShellParser
    shellparser.y
    "${CMAKE_CURRENT_BINARY_DIR}/shellparser.c"
    COMPILE_FLAGS "-Wall -Werror --report=all")

if(HAVE_READ AND HAVE_FILENO)
    set(flex_mode "--fast")
else()
    set(flex_mode "--nounistd")
endif()

flex_target(ShellScanner
    shellscanner.l
    "${CMAKE_CURRENT_BINARY_DIR}/shellscanner.c"
    COMPILE_FLAGS
    "${flex_mode} --header-file=${CMAKE_CURRENT_BINARY_DIR}/shellscanner.h")
add_flex_bison_dependency(ShellScanner ShellParser)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" sources)
set(sources ${sources} ${BISON_ShellParser_OUTPUTS} ${FLEX_ShellScanner_OUTPUTS})

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
