set(CMAKE_BUILD_TYPE RELEASE CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "RELEASE" "DEBUG")

set(VERSION ${PROJECT_VERSION})
configure_file(version.h.in version.h @ONLY)

add_library(msmelpwb SHARED msmelpwb.c params.c)
set_target_properties(msmelpwb PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(msmelpwb PROPERTIES INSTALL_RPATH "$ORIGIN;$ORIGIN/../../lib" BUILD_WITH_INSTALL_RPATH 1)
target_include_directories(msmelpwb PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(msmelpwb PRIVATE melpwb ${MEDIASTREAMER2_LIBRARIES} ${ORTP_LIBRARIES} ${BCTOOLBOX_CORE_LIBRARIES})
target_link_options(msmelpwb PRIVATE "LINKER:--exclude-libs=ALL")
install(TARGETS msmelpwb DESTINATION ${MS2_PLUGINS_DIR})
