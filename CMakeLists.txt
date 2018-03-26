cmake_minimum_required(VERSION 3.0)
project(flightgoggles_client_bindings)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )

set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

# Fix cuda linking issues
set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)

# Find OpenCV
find_package( OpenCV REQUIRED )

# Find Eigen
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

# Only compile ROS client if ROS is installed.
find_package(roscpp QUIET)
if(NOT roscpp_FOUND)
  message(STATUS "Couldn't find roscpp, so not building ROSClient. Did you source the ROS setup.sh file?")
  set(COMPILE_ROSCLIENT OFF)
else()
  message(STATUS "Found roscpp installed at ${roscpp_DIR}, so building ROSClient.")
  set(COMPILE_ROSCLIENT ON)

  # Init Catkin
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
endif()

add_subdirectory(src)


