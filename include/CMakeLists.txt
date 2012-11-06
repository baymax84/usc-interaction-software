file(GLOB old_hdrs "opencv/*.h*")
install(FILES ${old_hdrs}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${OPENCV_INCLUDE_INSTALL_PATH}/opencv
    COMPONENT main)
install(FILES "opencv2/opencv.hpp"
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${OPENCV_INCLUDE_INSTALL_PATH}/opencv2
    COMPONENT main)


