cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
#rosbuild_genmsg()
#uncomment if you have defined services
#rosbuild_gensrv()

#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)
#rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME} thread)
#rosbuild_add_executable(example examples/example.cpp)
#target_link_libraries(example ${PROJECT_NAME})

#build servo_controller_main
add_executable(servo_controller_main src/servo_controller_main.cpp src/servo_controller.cpp src/servo.cpp)

#build angle_servo_controller_main
add_executable(servo_angle_controller_main src/servo_angle_controller_main.cpp src/servo_angle_controller.cpp src/servo_controller.cpp src/servo.cpp)

#REMOVE THIS AFTER sparky_controller.cpp IS DONE
add_library(sparky src/servo_angle_controller.cpp src/servo_controller.cpp src/servo.cpp)

#build sparky_controller_main
###add_executable(sparky_controller_main src/sparky_controller_main.cpp src/sparky_controller.cpp src/servo_controller.cpp src/servo.cpp)
###add_library(sparky src/sparky_controller.cpp src/servo_angle_controller.cpp src/servo_controller.cpp src/servo.cpp)

#build yaml_parser
add_executable(yaml_parser src/yaml_parser.cpp src/servo_angle_controller.cpp src/servo_controller.cpp src/servo.cpp)
target_link_libraries(yaml_parser yaml-cpp)

