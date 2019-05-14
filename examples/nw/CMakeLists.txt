cmake_minimum_required(VERSION 3.13)

get_directory_property( hasParent PARENT_DIRECTORY )
if( NOT hasParent )
	set(CMAKE_CXX_STANDARD 17)
    add_subdirectory(../.. ${CMAKE_CURRENT_BINARY_DIR}/mart-common)
endif()


add_executable( mart-common_ex_udp_sender main.cpp )
target_link_libraries( mart-common_ex_udp_sender PUBLIC Mart::common)