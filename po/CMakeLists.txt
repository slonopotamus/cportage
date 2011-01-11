find_package(Gettext REQUIRED)

find_program(GETTEXT_XGETTEXT_EXECUTABLE xgettext)
if(NOT GETTEXT_XGETTEXT_EXECUTABLE)
    message(FATAL_ERROR "xgettext program not found")
endif()

set(potfile "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.pot")

file(GLOB_RECURSE sources
    RELATIVE "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/src/*.c")
add_custom_command(
    OUTPUT "${potfile}"
    COMMAND "${GETTEXT_XGETTEXT_EXECUTABLE}"
        --keyword=_
        --sort-output
        --no-wrap
        --add-comments
        --add-location
        --from-code=UTF-8
        --package-name=${CMAKE_PROJECT_NAME}
        --package-version=${CP_VERSION}
        -language=C
        --output="${potfile}"
        --default-domain=${CMAKE_PROJECT_NAME}
        --foreign-user
        --copyright-holder="Marat Radchenko"
        ${sources}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Extracting translation strings from sources into ${potfile}"
)
add_custom_target(${CMAKE_PROJECT_NAME}.pot DEPENDS "${potfile}")

file(GLOB translation_files *.po)
gettext_create_translations("${potfile}" ALL ${translation_files})
