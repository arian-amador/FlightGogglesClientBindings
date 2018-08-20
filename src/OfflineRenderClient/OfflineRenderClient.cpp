/**
 * @file   OfflineRenderClient.cpp
 * @author Winter Guerra
 * @brief  Pulls images from Unity and saves them as PNGs
 *space.
 **/

#include "OfflineRenderClient.hpp"

// 2 remote instances, 1 local instance.
#define NUM_FG_INSTANCES 3



///////////////////////
// Constructors
///////////////////////

OfflineRenderClient::OfflineRenderClient(std::filesystem::path trajectoryPath, std::filesystem::path flightGogglesConfigPath){

  renderDir = std::filesystem::path("/media/medusa/NVME_Data/temp/");
  outputDir = trajectoryPath.parent_path() / "images";
  
  // Define cameras
  unity_outgoing::Camera_t camL;
  camL.ID = "Camera_L";
  camL.channels = 1;
  camL.isDepth = false;
  camL.outputIndex = 0;

  unity_outgoing::Camera_t camR;
  camR.ID = "Camera_R";
  camR.channels = 1;
  camR.isDepth = false;
  camR.outputIndex = 0;

  unity_outgoing::Camera_t camD;
  camD.ID = "Camera_D";
  camD.channels = 1;
  camD.isDepth = false;
  camD.outputIndex = 0;

  // Save cameras
  cameras.push_back(camL);
  cameras.push_back(camR);
  cameras.push_back(camD);

  // // Add cameras to persistent state
  // flightGoggles.state.cameras.push_back(camL);
  // flightGoggles.state.cameras.push_back(camR);
}



////////////////////////////////////
// Example consumers and publishers
////////////////////////////////////

void imageConsumer(OfflineRenderClient *self){
    while (true){
      // Wait for render result (blocking).
      unity_incoming::RenderOutput_t renderOutput = self->flightGoggles.handleImageResponse();

      // Save render result
      for (int i = 0; i < renderOutput.images.size(); i++ ){
        std::string filename;
        filename.append
      }
    }
}

void posePublisher(OfflineRenderClient *self){
  // Sends render requests to FlightGoggles indefinitely
  while (true){
    // Update camera position
    self->updateCameraTrajectory();
    // Update timestamp of state message (needed to force FlightGoggles to rerender scene)
    self->flightGoggles.state.utime = self->flightGoggles.getTimestamp();
    // request render
    self->flightGoggles.requestRender();
    // Throttle requests to framerate.
    usleep(1e6/self->flightGoggles.state.maxFramerate);
    }
}


// Do a simple circular trajectory
void OfflineRenderClient::updateCameraTrajectory(){
  double period = 15.0f;
  double r = 2.0f;
  double t = (flightGoggles.getTimestamp()-startTime)/1000000.0f;
  double theta = -((t/period)*2.0f*M_PI);
  
  Transform3 cameraPose;
  cameraPose.translation() = Vector3(r*cos(theta), r*sin(theta), 1.5f);
  // Set rotation matrix using pitch, roll, yaw
  cameraPose.linear() = Eigen::AngleAxisd(theta-M_PI, Eigen::Vector3d(0,0,1)).toRotationMatrix();

  // Populate status message with new pose
  flightGoggles.setCameraPoseUsingROSCoordinates(cameraPose, 0);
  flightGoggles.setCameraPoseUsingROSCoordinates(cameraPose, 1);
}

///////////////////////
// Example Client Node
///////////////////////

int main() {

  // 


  // Create client
  OfflineRenderClient OfflineRenderClient;

  // Read parameters from json files
  OfflineRenderClient.flightGoggles.state.maxFramerate = 60; 
  OfflineRenderClient.flightGoggles.state.sceneFilename = "Hazelwood_Loft_Full_Night";
  
  // Instantiate RGBD cameras
  OfflineRenderClient.addCameras();
  
  // Fork sample render request thread
  // will request a simple circular trajectory
  std::thread posePublisherThread(posePublisher, &OfflineRenderClient);

  // Fork a sample image consumer thread
  std::thread imageConsumerThread(imageConsumer, &OfflineRenderClient);

  // Spin
  while (true) {sleep(1);}

  return 0;
}
