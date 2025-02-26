execute_process(COMMAND sh -c "cat /etc/os-release | sed -ne 's/^ID=//p'"
                OUTPUT_VARIABLE DISTRO
                OUTPUT_STRIP_TRAILING_WHITESPACE)

add_library(melp STATIC IMPORTED GLOBAL)
set_target_properties(melp PROPERTIES
                      IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/libmelp-${DISTRO}.a
                      INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include)

