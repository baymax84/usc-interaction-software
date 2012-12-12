# gather all sources in current dir using relative filenames

file( GLOB PROJECT_LIBRARY_SOURCE_ITEMS "*" )

foreach( library_source_item ${PROJECT_LIBRARY_SOURCE_ITEMS} )
    if( IS_DIRECTORY ${library_source_item} )
        get_filename_component( library_name ${library_source_item} NAME )

        add_subdirectory( ${library_source_item} )

        #get_target_property( library_path ${library_name} LOCATION )
        #get_filename_component( library_suffix ${library_path} EXT )
        #set_target_properties( ${library_name} PROPERTIES SUFFIX "${library_suffix}.${PROJECT_VERSION}" )
        set_target_properties( ${library_name} PROPERTIES VERSION ${PROJECT_VERSION} )
        set_target_properties( ${library_name} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} )

        list( APPEND PROJECT_LIBRARY_NAMES "${library_name}" )
        list( APPEND PROJECT_LIBRARIES_LFLAGS "-l${library_name}" )

        message( STATUS "added library ${library_name}" )
    endif()
endforeach()

set( PROJECT_LIBRARY_NAMES ${PROJECT_LIBRARY_NAMES} PARENT_SCOPE )
set( PROJECT_LIBRARIES_LFLAGS ${PROJECT_LIBRARIES_LFLAGS} PARENT_SCOPE )
