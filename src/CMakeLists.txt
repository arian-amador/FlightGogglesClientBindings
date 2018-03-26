# Always compile these dirs
add_subdirectory(Common)
add_subdirectory(GeneralClient)

# Only compile ROS client if ROS is installed.
find_package(roscpp QUIET)
if(NOT roscpp_FOUND)
  message(STATUS "Couldn't find roscpp, so not building ROSClient. Did you source the ROS setup.sh file?")
else()
  message(STATUS "Found roscpp installed at ${roscpp_DIR}, so building ROSClient.")
  add_subdirectory(ROSClient)
endif()