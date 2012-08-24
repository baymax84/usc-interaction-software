cmake_minimum_required( VERSION 2.6 )

set( PROJECT_VERSION unstable )

project( probabilistics )

set( PROJECT_EXECUTABLE_DIR bin )
set( PROJECT_INCLUDE_DIR include/interaction/${PROJECT_NAME} )
set( PROJECT_LIBRARY_DIR lib )

set( PROJECT_MAIN_LIB interaction-${PROJECT_NAME}-${PROJECT_VERSION} )

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/${PROJECT_EXECUTABLE_DIR} )
set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/${PROJECT_LIBRARY_DIR} )

include( FindPkgConfig )

pkg_check_modules( GSL REQUIRED gsl )
include_directories( ${GSL_INCLUDE_DIRS} )
link_directories( ${GSL_LIBRARY_DIRS} )

add_definitions( -std=c++0x )

include_directories( include )

add_subdirectory( include )
add_subdirectory( src )

enable_testing()
add_subdirectory( tests )
