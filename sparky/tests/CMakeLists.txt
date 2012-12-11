# gather all sources in current dir using relative filenames
file( GLOB TEST_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.cc *.c )

foreach( source ${TEST_SOURCES} )
    get_filename_component( source_name_base ${source} NAME_WE )
    set( source_name ${source_name_base} )
    set( source_src ${source} )

    add_executable( ${source_name} ${source_src} )
    target_link_libraries( ${source_name} ${PROJECT_MAIN_LIB} )
    add_test( ${source_name}_runs ${EXECUTABLE_OUTPUT_PATH}/${source_name} )
    #install( TARGETS ${source_name} DESTINATION /usr/${PROJECT_EXECUTABLE_DIR} )
endforeach()
