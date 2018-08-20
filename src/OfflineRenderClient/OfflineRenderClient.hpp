#ifndef OfflineRenderClient_H
#define OfflineRenderClient_H
/**
 * @file   OfflineRenderClient.hpp
 * @author Winter Guerra
 * @brief  Given a list of CSV camera poses in global frame and a state JSON, render the images to disk.
 */

#include <FlightGogglesClient.hpp>
// #include <jsonMessageSpec.hpp>

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <thread>

class OfflineRenderClient {
 public:
  // FlightGoggles interface object
  //FlightGogglesClient flightGoggles;

  // constructor
  OfflineRenderClient();

  // List of cameras to render
  std::vector<unity_outgoing::Camera_t> cameras;

  // List of FlightGoggles workers 
  std::vector<OfflineRenderWorker> workers;

  // Directories
  std::filesystem::path renderDir;
  std::filesystem::path outputDir;
  
};

class OfflineRenderWorker {
   public:
  // FlightGoggles interface object
  FlightGogglesClient flightGoggles;

  // constructor
  OfflineRenderWorker(unity_outgoing::Camera_t renderCamera, int instance_num, std::filesystem::path _renderDir);

  // Directories
  std::filesystem::path renderDir;
}

#endif
