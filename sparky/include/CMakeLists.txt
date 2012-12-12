# set include paths
file( GLOB INCLUDE_PATHS "*" )

foreach( include_path ${INCLUDE_PATHS} )
    if( IS_DIRECTORY ${include_path} )
        install( DIRECTORY ${include_path} DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_INCLUDE_INSTALL_PATH}" )
        message( STATUS "will install ${include_path} to ${CMAKE_INSTALL_PREFIX}/${PROJECT_INCLUDE_INSTALL_PATH}" )
    endif()
endforeach()
