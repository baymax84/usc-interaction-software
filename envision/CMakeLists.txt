cmake_minimum_required( VERSION 2.6 )

# set version info
set( PROJECT_VERSION_MAJOR 0 )
set( PROJECT_VERSION_MINOR 0.1 )

set( PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )

# set project name
project( envision )

# set some common paths
set( PROJECT_EXECUTABLE_DIR bin )
set( PROJECT_INCLUDE_BASEPATH include )
set( PROJECT_INCLUDE_DIR ${PROJECT_INCLUDE_BASEPATH}/envision )
set( PROJECT_LIBRARY_DIR lib )

set( PROJECT_MAIN_LIB ${PROJECT_NAME} )

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/${PROJECT_EXECUTABLE_DIR} )
set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/${PROJECT_LIBRARY_DIR} )

# configure pkgconfig templates
file( GLOB_RECURSE PROJECT_PKGCONFIG_TEMPLATES ${PROJECT_SOURCE_DIR}/pkgconfig *.pc.in )
foreach( pkgconfig_template ${PROJECT_PKGCONFIG_TEMPLATES} )
  get_filename_component( pkgconfig_template_basename ${pkgconfig_template} NAME_WE )
  configure_file( ${pkgconfig_template} ${PROJECT_SOURCE_DIR}/pkgconfig/${pkgconfig_template_basename}.pc @ONLY )
endforeach()

# use pkgconfig
include( FindPkgConfig )

# import opencv
pkg_check_modules( OPENCV REQUIRED opencv-2.3.1 )
include_directories( ${OPENCV_INCLUDE_DIRS} )
link_directories( ${OPENCV_LIBRARY_DIRS} )

# set list of dependent libraries
set( PROJECT_DEPENDENT_LIBRARIES ${OPENCV_LIBRARIES} )

# set include paths
include_directories( include )

add_definitions( "-std=c99" )

include( CheckTypeSize )

check_type_size( "int" SIZEOF_INT )
check_type_size( "long" SIZEOF_LONG )
check_type_size( "long long" SIZEOF_LONG_LONG )

message( "int: ${SIZEOF_INT}, long: ${SIZEOF_LONG}, long long: ${SIZEOF_LONG_LONG}" )

configure_file( ${PROJECT_SOURCE_DIR}/${PROJECT_INCLUDE_DIR}/env_config.h.in ${PROJECT_SOURCE_DIR}/${PROJECT_INCLUDE_DIR}/env_config.h )

# add project components
add_subdirectory( src )

add_custom_target( test-results COMMAND echo "No tests to run" )
