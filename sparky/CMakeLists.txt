cmake_minimum_required( VERSION 2.6 )

# set version info
set( PROJECT_VERSION_MAJOR 0 )
set( PROJECT_VERSION_MINOR 0.1 )

set( PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )

# set project name
project( sparky )

# set some common paths
set( PROJECT_EXECUTABLE_DIR bin )
set( PROJECT_INCLUDE_BASEPATH include/interaction )
set( PROJECT_INCLUDE_DIR ${PROJECT_INCLUDE_BASEPATH}/${PROJECT_NAME} )
set( PROJECT_LIBRARY_DIR lib )

set( PROJECT_MAIN_LIB interaction-${PROJECT_NAME} )

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

# import gsl
pkg_check_modules( GSL REQUIRED gsl )
include_directories( ${GSL_INCLUDE_DIRS} )
link_directories( ${GSL_LIBRARY_DIRS} )

# set list of dependent libraries
set( PROJECT_DEPENDENT_LIBRARIES ${GSL_LIBRARIES} )

# set include paths
include_directories( include )

# add project components
add_subdirectory( include )
add_subdirectory( src )

enable_testing()
add_subdirectory( tests )
