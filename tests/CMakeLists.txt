macro(add_cportage_test _test_name _test_source)
	add_executable(${_test_name} ${_test_source})
	target_link_libraries(${_test_name} cportage)
	add_test(${_test_name} ${_test_name})
endmacro()

add_cportage_test(atom_test atom_test.c)
