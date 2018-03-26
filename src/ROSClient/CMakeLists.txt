
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





