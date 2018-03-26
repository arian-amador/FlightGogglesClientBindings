/**
 * @file   GeneralClient.cpp
 * @author Winter Guerra
 * @brief  Pulls images from Unity and saves them as PNGs
 *space.
 **/

#include "GeneralClient.hpp"

#define SHOW_DEBUG_IMAGE_FEED false


///////////////////////
// Constructors
///////////////////////

GeneralClient::GeneralClient(){
}



////////////////////////////////////
// Example consumers and publishers
////////////////////////////////////

void imageConsumer(GeneralClient *self){
    while (true){
      // Wait for render result (blocking).
      unity_incoming::RenderOutput_t renderOutput = self->flightGoggles.handleImageResponse();

      // Display result
      if (SHOW_DEBUG_IMAGE_FEED){
        cv::imshow("Debug RGB", renderOutput.images[0]);
        cv::imshow("Debug D", renderOutput.images[1]);
        cv::waitKey(1);
      }
    }
}

void posePublisher(GeneralClient *self){
  // Sends render requests to FlightGoggles indefinitely
  while (true){
    // Update timestamp of state message (needed to force FlightGoggles to rerender scene)
    self->flightGoggles.state.utime = self->flightGoggles.getTimestamp();
    // request render
    self->flightGoggles.requestRender();
    // Throttle requests to framerate.
    usleep(1e6/self->flightGoggles.state.maxFramerate);
    }
}

///////////////////////
// Example Client Node
///////////////////////

int main() {
  // Create client
  GeneralClient generalClient;

  // Set some parameters.

  // Prepopulate FlightGoggles state with camera pose
  Transform3 camera_pose;
  camera_pose.translation() = Vector3(0,-1,1);
  // Set rotation matrix using pitch, roll, yaw
  camera_pose.linear() = Eigen::AngleAxisd(M_PI/4.0f, Eigen::Vector3d(0,0,0)).toRotationMatrix();

  // Populate status message with new pose
  generalClient.flightGoggles.setCameraPoseUsingROSCoordinates(camera_pose, 0);
  generalClient.flightGoggles.setCameraPoseUsingROSCoordinates(camera_pose, 1);

  // Fork sample render request thread
  std::thread posePublisherThread(posePublisher, &generalClient);

  // Fork a sample image consumer thread
  std::thread imageConsumerThread(imageConsumer, &generalClient);

  // Spin
  while (true) {sleep(1);}

  return 0;
}
