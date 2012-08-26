cmake_minimum_required( VERSION 2.6 )

set( PROJECT_RELEASE_MAJOR 0 )
set( PROJECT_RELEASE_MINOR 1 )

set( PROJECT_VERSION_MAJOR 0 )
set( PROJECT_VERSION_MINOR 0.1 )

set( PROJECT_RELEASE ${PROJECT_RELEASE_MAJOR}.${PROJECT_RELEASE_MINOR} )
set( PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )

project( probabilistics )

set( PROJECT_EXECUTABLE_DIR bin )
set( PROJECT_INCLUDE_DIR include/interaction/${PROJECT_NAME} )
set( PROJECT_LIBRARY_DIR lib )

set( PROJECT_MAIN_LIB interaction-${PROJECT_NAME}-${PROJECT_RELEASE_MAJOR} )

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/${PROJECT_EXECUTABLE_DIR} )
set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/${PROJECT_LIBRARY_DIR} )

include( FindPkgConfig )

pkg_check_modules( GSL REQUIRED gsl )
include_directories( ${GSL_INCLUDE_DIRS} )
link_directories( ${GSL_LIBRARY_DIRS} )

set( PROJECT_DEPENDENT_LIBRARIES ${GSL_LIBRARIES} )
add_definitions( -std=c++0x )

include_directories( include )

add_subdirectory( include )
add_subdirectory( src )

enable_testing()
add_subdirectory( tests )
