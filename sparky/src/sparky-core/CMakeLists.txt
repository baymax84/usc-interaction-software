file( GLOB_RECURSE ${library_name}_SOURCES *.cpp *.cc *.c )
add_library( ${library_name} SHARED ${${library_name}_SOURCES} )

# import yaml-cpp
pkg_check_modules( YAML_CPP REQUIRED yaml-cpp )
include_directories( ${YAML_CPP_INCLUDE_DIRS} )
link_directories( ${YAML_CPP_LIBRARY_DIRS} )

target_link_libraries( ${library_name} ${YAML_CPP_LIBRARIES} )

install( TARGETS ${library_name} DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_LIBRARY_INSTALL_PATH}" )
message( STATUS "will install ${library_name} to ${CMAKE_INSTALL_PREFIX}/${PROJECT_LIBRARY_INSTALL_PATH}" )
