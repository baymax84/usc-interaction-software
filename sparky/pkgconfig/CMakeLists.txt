# configure pkgconfig templates
file( GLOB_RECURSE PROJECT_PKGCONFIG_TEMPLATES ${CMAKE_CURRENT_SOURCE_DIR} "*.pc.in" )
foreach( pkgconfig_template ${PROJECT_PKGCONFIG_TEMPLATES} )
  get_filename_component( pkgconfig_template_basename ${pkgconfig_template} NAME_WE )
  configure_file( ${pkgconfig_template} "${PROJECT_BINARY_DIR}/pkgconfig/${pkgconfig_template_basename}.pc" @ONLY )
  install( FILES "${PROJECT_BINARY_DIR}/pkgconfig/${pkgconfig_template_basename}.pc" DESTINATION "${CMAKE_INSTALL_PREFIX}/${PROJECT_LIBRARY_INSTALL_PATH}/pkgconfig" )
  message( STATUS "will install ${PROJECT_BINARY_DIR}/pkgconfig/${pkgconfig_template_basename}.pc to ${CMAKE_INSTALL_PREFIX}/${PROJECT_LIBRARY_INSTALL_PATH}/pkgconfig" )
endforeach()
