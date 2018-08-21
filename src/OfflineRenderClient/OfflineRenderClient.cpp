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

OfflineRenderClient::OfflineRenderClient(std::string environmentString,unity_outgoing::Camera_t _renderCam, int _instanceNum, std::string _trajectoryPath, Vector7d _poseOffset, std::string _renderDir){
  // Save params
  poseOffset = _poseOffset;
  renderDir = _renderDir;

  // Init state vars
  renderQueueLength = 0;
  allFramesRequested = false;
  
  
  // Spawn flightGoggles connection
  flightGoggles = FlightGogglesClient(_instanceNum);

  // Get the pose list from CSV.
  io::CSVReader<8> _csv(_trajectoryPath);
  csv = &_csv;
  csv->set_header("utime", "x","y","z","quat_x","quat_y","quat_z","quat_w");

  // // Add cameras to persistent state
  flightGoggles.state.cameras.push_back(_renderCam);
  // Set environment
  flightGoggles.state.sceneFilename = environmentString;
}



// Read from the CSV
void OfflineRenderClient::updateCameraPose(Vector7d csvPose){


  Transform3 cameraPose;
  cameraPose.fromPositionOrientationScale(Vector3(csvPose[0],csvPose[1],csvPose[2]), Quaternionx(csvPose[3],csvPose[4],csvPose[5],csvPose[6]), Vector3(1, 1, 1));
//  cameraPose.translation() = Vector3(r*cos(theta), r*sin(theta), 1.5f);
//  // Set rotation matrix using pitch, roll, yaw
//  cameraPose.linear() = Eigen::AngleAxisd(theta-M_PI, Eigen::Vector3d(0,0,1)).toRotationMatrix();

  // Populate status message with new pose
  flightGoggles.setCameraPoseUsingNEDCoordinates(cameraPose, 0);

}




////////////////////////////////////
// Example consumers and publishers
////////////////////////////////////

void imageConsumer(OfflineRenderClient *self){
    while (!(self->allFramesRequested && self->renderQueueLength<=0)){
      // Wait for render result (blocking).
      unity_incoming::RenderOutput_t renderOutput = self->flightGoggles.handleImageResponse();

      // Save render result
      for (int i = 0; i < renderOutput.images.size(); i++ ){
        fs::path filename;
        filename += self->renderDir;
        filename /= std::to_string(renderOutput.renderMetadata.utime) + std::string("_") + renderOutput.renderMetadata.cameraIDs[i] + std::string(".ppm");
        cv::imwrite(filename, renderOutput.images[i]);
      }

      // Update number of outstanding render requests.
      std::unique_lock<std::mutex> lk(self->mutexForRenderQueue); // temp lock
      self->renderQueueLength--;
      lk.unlock();
      self->renderQueueBelowCapacity.notify_all();
    }
}

void posePublisher(OfflineRenderClient *self){
  // Sends render requests to FlightGoggles from CSV
  int64_t utime;
  double x,y,z,quat_x,quat_y,quat_z,quat_w;

  while (self->csv->read_row(utime,x,y,z,quat_x,quat_y,quat_z,quat_w)){
    // Wait for render queue to empty before requesting more frames
    std::unique_lock<std::mutex> lk(self->mutexForRenderQueue);
    self->renderQueueBelowCapacity.wait(lk, [self]{return (self->renderQueueLength < self->renderQueueMaxLength);});

    Vector7d csvPose;
    csvPose << x,y,z,quat_x,quat_y,quat_z,quat_w;

    // Update camera position
    self->updateCameraPose(csvPose);

    self->flightGoggles.state.utime = utime;
    // request render
    self->flightGoggles.requestRender();

    // Update render queue
    lk.lock();
    self->renderQueueLength++;
    lk.unlock();
    }
  
  self->allFramesRequested = true;
}

// Master worker thread
void OfflineRenderClientThread(std::string environmentString, unity_outgoing::Camera_t _renderCam, int _instanceNum, std::string _trajectoryPath, Vector7d _poseOffset, std::string _renderDir){

    // Create client object
    OfflineRenderClient worker(environmentString,_renderCam, _instanceNum, _trajectoryPath, _poseOffset, _renderDir);

    // Spawn worker threads
    // Fork sample render request thread
    // will request a simple circular trajectory
    std::thread posePublisherThread(posePublisher, &worker);

    // Fork a sample image consumer thread
    std::thread imageConsumerThread(imageConsumer, &worker);

    // Wait for threads to finish.
    posePublisherThread.join();
    imageConsumerThread.join();

}


///////////////////////
// Example Client Node
///////////////////////

int main(int argc, char **argv) {

  // Get args
  if (argc != 6) {
    std::cerr << "Please provide args! ex: ./OfflineRenderClient Butterfly_World 0.0 0.1 -0.25 poseList.csv" << std::endl;
    std::cerr << "Note that offset arg is in NED." << std::endl;
    return 1;
  }
  std::string environment_string = argv[1];
  Vector7d environmentPoseOffset;
  environmentPoseOffset << std::stod(argv[2]),std::stod(argv[3]),std::stod(argv[4]),0,0,0,0;
  std::string trajectoryPath(argv[5]);

  // Specify render output path.
  std::string renderDir = "/media/medusa/NVME_Data/temp/";
  
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

  std::vector<unity_outgoing::Camera_t> cameras;

  // Save cameras
  cameras.push_back(camL);
  cameras.push_back(camR);
  cameras.push_back(camD);

  // spawn parallel render clients for each camera
  std::vector<std::thread> workers;
  workers.reserve(NUM_FG_INSTANCES);

  for (int i = 0; i < NUM_FG_INSTANCES; i++){
      std::thread worker(OfflineRenderClientThread, environment_string, cameras[i], i, trajectoryPath, environmentPoseOffset, renderDir);

      workers.push_back(std::move(worker));

  }

  // Wait for worker threads to die
  for (auto& t : workers)
      t.join();




  // // Create client
  // OfflineRenderClient OfflineRenderClient;

  // // Read parameters from json files
  // OfflineRenderClient.flightGoggles.state.maxFramerate = 60; 
  // OfflineRenderClient.flightGoggles.state.sceneFilename = "Hazelwood_Loft_Full_Night";
  
  // // Instantiate RGBD cameras
  // OfflineRenderClient.addCameras();
  
  // // Fork sample render request thread
  // // will request a simple circular trajectory
  // std::thread posePublisherThread(posePublisher, &OfflineRenderClient);

  // // Fork a sample image consumer thread
  // std::thread imageConsumerThread(imageConsumer, &OfflineRenderClient);

  // // Spin
  // while (true) {sleep(1);}

  return 0;
}
