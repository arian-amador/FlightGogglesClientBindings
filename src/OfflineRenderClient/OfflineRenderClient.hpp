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
#include <experimental/filesystem>
#include <string>
#include <vector>

#include "csv.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

namespace fs = std::experimental::filesystem;

class OfflineRenderClient {
 public:
  // FlightGoggles interface object
  FlightGogglesClient* flightGoggles;

  // constructor
  OfflineRenderClient(std::string environmentString,unity_outgoing::Camera_t _renderCam, int _instanceNum, std::string _trajectoryPath, Vector7d _poseOffset, std::string _renderDir);

  // PoseList CSV
  io::CSVReader<8>* csv;

  Vector7d poseOffset;

  // Directories
  std::string renderDir;

  // Keep track of outstanding render requests
  std::mutex mutexForRenderQueue;
  std::condition_variable renderQueueBelowCapacity;
  std::atomic<int> renderQueueLength;
  const int renderQueueMaxLength = 150;

  std::atomic<bool> allFramesRequested;

  // Methods
  void updateCameraPose(Vector7d csvPose);

};


#endif
