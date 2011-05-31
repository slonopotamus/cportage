aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" sources)

add_executable(cportageq ${sources})
set_link_flags(cportageq)
target_link_libraries(cportageq cportage)

install(TARGETS cportageq RUNTIME DESTINATION bin)
