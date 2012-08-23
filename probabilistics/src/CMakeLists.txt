# gather all sources in current dir using relative filenames
file( GLOB LIB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.cc *.c )
add_library( ${PROJECT_MAIN_LIB} SHARED ${LIB_SOURCES} )
target_link_libraries( ${PROJECT_MAIN_LIB} ${GSL_LIBRARIES} )
install( TARGETS ${PROJECT_MAIN_LIB} DESTINATION /usr/${PROJECT_LIBRARY_DIR} )
