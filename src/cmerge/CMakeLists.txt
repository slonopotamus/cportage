configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

# Executables

add_definitions(-DG_LOG_DOMAIN="cmerge")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB cmerge_sources *.c)

add_executable(cmerge ${cmerge_sources})
set_link_flags(cmerge)
target_link_libraries(cmerge cportage)

install(TARGETS cmerge RUNTIME DESTINATION bin)
