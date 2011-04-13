include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../src")

macro(add_cportage_test _test_name)
  add_executable(${_test_name} ${_test_name}.c)
  set_link_flags(${_test_name})
  target_link_libraries(${_test_name} cportage_static ${GLIB2_LIBRARIES})
  add_test(${_test_name} ${_test_name} "${CMAKE_CURRENT_SOURCE_DIR}")
endmacro()

add_cportage_test(atom_test)
add_cportage_test(strings_test)
add_cportage_test(shellconfig_test)
add_cportage_test(version_test)
add_cportage_test(settings_test)
