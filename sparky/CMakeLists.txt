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

#build test_sparky
add_executable(test_sparky src/test.cc)

#build sine
add_executable(sine src/sine_main.cc src/controller.cc src/pololu.cc src/utility.cc)
target_link_libraries(sine pthread m rt)

#build editor
include($ENV{ROS_ROOT}/core/rosbuild/FindPkgConfig.cmake)
pkg_check_modules(GTKMM REQUIRED gtkmm-2.4)
include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
add_executable(editor src/editor_main.cc src/bez_curve.cc src/comp.cc src/curve_pane.cc src/curve_set.cc src/init.cc src/maestro_servo_controller.cpp src/main_frame.cc src/player.cc src/pololu.cc src/scroller.cc src/selector.cc src/servo.cc src/time_bar.cc src/utility.cc)
target_link_libraries(editor ${GTKMM_LIBRARIES} gthread-2.0 pthread m rt)

#build maestro_servo_controller_main
add_executable(maestro_servo_controller_main src/maestro_servo_controller_main.cpp src/maestro_servo_controller.cpp)

#build sparky
add_library(sparky src/maestro_servo_controller.cpp)
#add_library(sparky src/sparky_controller.cpp src/maestro_servo_controller.cpp)
#add_library(sparky src/controller.cc src/pololu.cc)
#add_executable(sparky_controller_main src/sparky_controller_main.cpp)
