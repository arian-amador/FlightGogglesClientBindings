#ifndef GENERALCLIENT_H
#define GENERALCLIENT_H
/**
 * @file   GeneralClient.hpp
 * @author Winter Guerra
 * @brief  Basic client interface for FlightGoggles.
 */

#include <FlightGogglesClient.hpp>
// #include <jsonMessageSpec.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <thread>

class GeneralClient {
 public:
  // FlightGoggles interface object
  FlightGogglesClient flightGoggles;

  // constructor
  GeneralClient();

  // Add RGBD camera settings to scene.
  void addCameras();

  // Do a simple trajectory with the camera
  void updateCameraTrajectory();

};

#endif
