cmake_minimum_required( VERSION 2.6 )

# set project name
set( PROJECT_DEFAULT_NAME          sparky CACHE STRING "The project name." )
# set version info
set( PROJECT_DEFAULT_VERSION_MAJOR 0.1    CACHE STRING "The major version number." )
set( PROJECT_DEFAULT_VERSION_MINOR 0      CACHE STRING "The minor version number." )
# set some options
set( CMAKE_INSTALL_PREFIX          "/usr" CACHE PATH "The path to install to." )
set( INSTALL_TO_MANGLED_PATHS      YES    CACHE BOOL "Include version in paths where applicable." )
set( BUILD_TESTS                   YES    CACHE BOOL "Compile test binaries." )
set( INSTALL_TESTS                 NO     CACHE BOOL "Install tests (implies BUILD_TESTS=YES)." )
set( PARSE_BUILDINFO               YES    CACHE BOOl "Look for and parse buildinfo file."

# override BUILD_TESTS if INSTALL_TESTS is enabled
if( INSTALL_TESTS )
    set( BUILD_TESTS YES )
endif()

# look for and parse buildinfo; note that if a buildinfo file containing relevant info is located, the defaults above may be overridden
if( PARSE_BUILDINFO )
    message( STATUS "Attempting to load parameters from buildinfo..." )
    if( EXISTS "../buildinfo" )
        file( READ "buildinfo" PROJECT_BUILDINFO )
        string( REGEX REPLACE "\n" "" PROJECT_BUILDINFO ${PROJECT_BUILDINFO} )
        message( STATUS "  buildinfo found with content: [${PROJECT_BUILDINFO}]" )

        string( REGEX REPLACE "^.*--project ([0-9a-zA-Z_\\-]+).*$" "\\1" PROJECT_BUILDINFO_NAME ${PROJECT_BUILDINFO} )
        message( STATUS "  project from buildinfo: [${PROJECT_BUILDINFO_NAME}]" )

        string( REGEX REPLACE "^.*--version ([0-9\\.]+).*$" "\\1" PROJECT_BUILDINFO_VERSION ${PROJECT_BUILDINFO} )
        string( REGEX REPLACE "([0-9]+\\.[0-9]+)\\.[0-9]+" "\\1" PROJECT_BUILDINFO_VERSION_MAJOR ${PROJECT_BUILDINFO_VERSION} )
        string( REGEX REPLACE "[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1" PROJECT_BUILDINFO_VERSION_MINOR ${PROJECT_BUILDINFO_VERSION} )
        message( STATUS "  version from buildinfo: [${PROJECT_BUILDINFO_VERSION}] ([${PROJECT_BUILDINFO_VERSION_MAJOR}].[${PROJECT_BUILDINFO_VERSION_MINOR}])" )
    else()
        message( STATUS "  No buildinfo found; using defaults." )
    endif()
endif()

# set version info
if( PROJECT_BUILDINFO_VERSION_MAJOR )
    set( PROJECT_VERSION_MAJOR ${PROJECT_BUILDINFO_VERSION_MAJOR} )
else()
    set( PROJECT_VERSION_MAJOR ${PROJECT_DEFAULT_VERSION_MAJOR} )
endif()
if( PROJECT_BUILDINFO_VERSION_MINOR )
    set( PROJECT_VERSION_MINOR ${PROJECT_BUILDINFO_VERSION_MINOR} )
else()
    set( PROJECT_VERSION_MINOR ${PROJECT_DEFAULT_VERSION_MINOR} )
endif()

# set project name
if( PROJECT_BUILDINFO_NAME )
    project( ${PROJECT_BUILDINFO_NAME} )
else()
    project( ${PROJECT_DEFAULT_NAME} )
endif()

# set version-related convenience vars
set( PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}" )
set( PROJECT_VERSIONNAME "${PROJECT_NAME}-${PROJECT_VERSION}" )

# set some common paths
set( EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin" )
set( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib" )
set( PROJECT_LIBRARY_INSTALL_PATH     "lib" )

# paths
if( INSTALL_TO_MANGLED_PATHS )
    set( PROJECT_INCLUDE_INSTALL_PATH "include/${PROJECT_VERSIONNAME}" )
    set( PROJECT_TESTS_INSTALL_PATH   "share/${PROJECT_VERSIONNAME}/tests" )
else()
    set( PROJECT_INCLUDE_INSTALL_PATH "include" )
    set( PROJECT_TESTS_INSTALL_PATH   "share/${PROJECT_NAME}/tests" )
endif()

# paths
set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PROJECT_LIBRARY_INSTALL_PATH}" )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

# add -I for everything under include
include_directories( include )

# import pkg-config stuff
include( FindPkgConfig )

# add project components
add_subdirectory( include )
add_subdirectory( src )
add_subdirectory( pkgconfig )

# add tests, if applicable
if( BUILD_TESTS )
    enable_testing()
    add_subdirectory( tests )
endif()
