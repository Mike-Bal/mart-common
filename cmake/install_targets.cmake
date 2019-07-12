
# https://cmake.org/cmake/help/latest/command/find_package.html

function(install_targets targets namespace )

set( INSTALL_DIR_CMAKE share/MartCommon )
set( CONFIG_STEM_NAME MartCommonConfig )

install( TARGETS ${targets} EXPORT ${CONFIG_STEM_NAME} )
install( EXPORT ${CONFIG_STEM_NAME}
	FILE
		${CONFIG_STEM_NAME}.cmake
	NAMESPACE
		${namespace}
	DESTINATION
		${INSTALL_DIR_CMAKE}
)
install( DIRECTORY include/ DESTINATION include )

include(CMakePackageConfigHelpers)

write_basic_package_version_file("${CONFIG_STEM_NAME}Version.cmake"
	VERSION ${${PROJECT_NAME}_VERSION}
	COMPATIBILITY SameMajorVersion
)

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_STEM_NAME}Version.cmake"
	DESTINATION
		${INSTALL_DIR_CMAKE}
)

install(
	FILES
		"LICENSE.md"
	DESTINATION
		share
)

endfunction()
