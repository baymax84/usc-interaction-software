# gather all sources in current dir using relative filenames
file( GLOB LIB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.cc *.c )

add_library( ${PROJECT_MAIN_LIB} SHARED ${LIB_SOURCES} )
target_link_libraries( ${PROJECT_MAIN_LIB} ${PROJECT_DEPENDENT_LIBRARIES} )

get_target_property( PROJECT_MAIN_LIB_PATH ${PROJECT_MAIN_LIB} LOCATION )
get_filename_component( PROJECT_MAIN_LIB_SUFFIX ${PROJECT_MAIN_LIB_PATH} EXT )
set_target_properties( ${PROJECT_MAIN_LIB} PROPERTIES SUFFIX "${PROJECT_MAIN_LIB_SUFFIX}.${PROJECT_VERSION}" )
