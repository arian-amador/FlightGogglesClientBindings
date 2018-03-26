#cmake_minimum_required(VERSION 2.8.3)
#project(flight_goggles_client_example)

## Compile as C++11, supported in ROS Kinetic and newer
#add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  tf
  tf2
  nav_msgs
  sensor_msgs
  geometry_msgs
)

#find_package( Eigen3 REQUIRED )


catkin_package(
    LIBRARIES simulator
    CATKIN_DEPENDS roscpp rospy std_msgs
    # DEPENDS system_lib
    tf
    tf2
)

# Include ROS dirs
include_directories(
  ${catkin_INCLUDE_DIRS}
)

# Compile ROS nodes
add_executable(ROSClient ROSClient.cpp)
target_link_libraries(ROSClient ${OpenCV_LIBS} FlightGogglesClientLib pthread ${catkin_LIBRARIES})

add_executable(trajectory_generation trajectory_generation.cpp)
target_link_libraries(trajectory_generation ${catkin_LIBRARIES})





