cmake_minimum_required(VERSION 2.8.3)
project(busroute)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS 
roscpp 
geometry_msgs 
move_base_msgs 
actionlib
actionlib_msgs
costmap_2d 
tf
ecl_threads
ecl_geometry 
ecl_linear_algebra 
kobuki_msgs 
kobuki_dock_drive 
nav_msgs
visualization_msgs
std_msgs
map_msgs
sensor_msgs
cv_bridge
image_transport
rospy
)


#find_package(OpenCV REQUIRED)
find_package(OpenCV REQUIRED)



catkin_package(
  CATKIN_DEPENDS
  roscpp 
  geometry_msgs 
  move_base_msgs  
  actionlib
  actionlib_msgs
  costmap_2d 
  tf
  ecl_threads
  ecl_geometry 
  ecl_linear_algebra 
  kobuki_msgs 
  kobuki_dock_drive 
  nav_msgs
  visualization_msgs
  std_msgs
  map_msgs
  geometry_msgs
  sensor_msgs
  cv_bridge
  image_transport
)


## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a run_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a run_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
# add_message_files(
#   FILES
#   Message1.msg
#   Message2.msg
# )

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate actions in the 'action' folder
# add_action_files(
#   FILES
#   Action1.action
#   Action2.action
# )

## Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   std_msgs  # Or other packages containing msgs
# )

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a run_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
##     and list every .cfg file to be processed

## Generate dynamic reconfigure parameters in the 'cfg' folder
# generate_dynamic_reconfigure_options(
#   cfg/DynReconf1.cfg
#   cfg/DynReconf2.cfg
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES busroute
#  CATKIN_DEPENDS other_catkin_pkg
#  DEPENDS system_lib
)

###########
## Build ##
###########

# c++11 support required
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()


## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
include
getSensor1
explore1
#task1
color
${catkin_INCLUDE_DIRS}

)
include_directories(${OpenCV_INCLUDE_DIR})

## Declare a C++ executable
#add_executable(auto_docking src/auto_docking.cpp)
#add_executable(getSensor src/getSensor.cpp)
#target_link_libraries(navigation mylib)

## Specify libraries to link a library or executable target against
#add_executable(busroute_node src/busroute.cpp)
#target_link_libraries(busroute_node
  #${catkin_LIBRARIES}
#)

#add_executable(navigation src/navigation.cpp)
#add_dependencies(navigation ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
add_library(myLib src/getSensor.cpp src/color.cpp )
#target_link_libraries(navigation
  #${catkin_LIBRARIES}
  #myLib
  #explore
#)

#add_executable(color_detect
#src/color.cpp
#)
#add_dependencies(color ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
#target_link_libraries(color_detect myLib ${catkin_LIBRARIES} ${OpenCV_LIBS})

add_executable(explore
  src/costmap_client.cpp
  src/explore.cpp
  src/frontier_search.cpp
  #src/color.cpp
    #src/tasks.cpp
)
add_dependencies(explore ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(explore myLib ${catkin_LIBRARIES})

add_executable(navi
  src/navigation.cpp
  #src/colordetect.cpp
  #src/tasks.cpp
  

)
add_dependencies(navi ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(navi myLib ${catkin_LIBRARIES} ${OpenCV_LIBS})

#add_executable(colordetecter
  #src/navigation.cpp
  #src/color.cpp
  #src/tasks.cpp
  

#)
#add_dependencies(colordetecter ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
#target_link_libraries(colordetecter myLib ${catkin_LIBRARIES} ${OpenCV_LIBS})

#############
## Install ##
#############

# install nodes
install(TARGETS explore
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
install(TARGETS navi
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
#install(TARGETS colordetecter
  #ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  #LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  #RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
#)

# install roslaunch files
install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
)


#############
## Testing ##
#############
if(CATKIN_ENABLE_TESTING)
  find_package(roslaunch REQUIRED)

  # test all launch files
  roslaunch_add_file_check(launch)
endif()

#target_link_libraries(getSensor
  #${catkin_LIBRARIES}
#)

#target_link_libraries(auto_docking
 # ${catkin_LIBRARIES}
#)


