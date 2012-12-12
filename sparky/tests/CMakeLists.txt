# gather all sources in current dir using relative filenames
file( GLOB TEST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.cc *.c )

message( STATUS "using project libraries: ${PROJECT_LIBRARY_NAMES}" )
message( STATUS "using project lflags: ${PROJECT_LIBRARIES_LFLAGS}" )

foreach( source ${TEST_SOURCES} )
    get_filename_component( source_name_base ${source} NAME_WE )
    set( source_name ${source_name_base} )
    set( source_src ${source} )

    add_executable( ${source_name} ${source_src} )
    target_link_libraries( ${source_name} sparky-core )
    add_test( ${source_name}_runs ${EXECUTABLE_OUTPUT_PATH}/${source_name} )
    if( INSTALL_TESTS )
        install( TARGETS ${source_name} DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_TESTS_INSTALL_PATH}" )
        message( STATUS "will install ${source_name} to ${CMAKE_INSTALL_PREFIX}/${PROJECT_TESTS_INSTALL_PATH}" )
    endif()
endforeach()
