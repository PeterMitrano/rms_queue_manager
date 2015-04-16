cmake_minimum_required(VERSION 2.8.3)
project(rms_queue_manager)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp std_msgs message_generation)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################


## Messages
add_message_files(
  FILES
  UserStatus.msg
  RMSQueue.msg
)

## Services
add_service_files(
  FILES
  UpdateQueue.srv
)

generate_messages(
  DEPENDENCIES
  std_msgs
)

## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(CATKIN_DEPENDS message_runtime)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

## Declare a cpp executable
add_executable(rms_queue_manager src/rms_queue_manager.cpp)

## Specify libraries to link a library or executable target against
target_link_libraries(rms_queue_manager
   ${catkin_LIBRARIES}
)

add_dependencies(rms_queue_manager rms_queue_manager_gencpp)

#############
## Install ##
#############

## Mark executables and/or libraries for installation
#install(TARGETS rms_queue_manager
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
#)

## Copy header files
#install(DIRECTORY include/${PROJECT_NAME}/
#  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
#)
