
# Include ROS dirs
include_directories(
  ${catkin_INCLUDE_DIRS}
)

# Compile ROS nodes
add_executable(ROSClient ROSClient.cpp)
target_link_libraries(ROSClient ${OpenCV_LIBS} FlightGogglesClientLib pthread ${catkin_LIBRARIES})

add_executable(trajectoryGeneration trajectoryGeneration.cpp)
target_link_libraries(trajectoryGeneration ${catkin_LIBRARIES})





