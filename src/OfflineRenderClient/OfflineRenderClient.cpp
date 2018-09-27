/**
 * @file   OfflineRenderClient.cpp
 * @author Winter Guerra
 * @brief  Pulls images from Unity and saves them as PNGs. Assumes that at least num_cameras FlightGoggles instances are running.
 *space.
 **/

#include "OfflineRenderClient.hpp"

///////////////////////
// Constructors
///////////////////////

OfflineRenderClient::OfflineRenderClient(std::string environmentString,unity_outgoing::Camera_t _renderCam, int _instanceNum, std::string _trajectoryPath, Transform3 _cameraPoseOffset, Transform3 _envTransform, std::string _renderDir){
  // Save params
  envTransform = _envTransform;
  body_T_camera = _cameraPoseOffset;
  renderDir = _renderDir;

  // set connection params
  allFramesRequested = false;
  connected = false;
  
  // Spawn flightGoggles connection

  flightGoggles = new FlightGogglesClient(_instanceNum);

  // Get the pose list from CSV.
  csv = new io::CSVReader<8>(_trajectoryPath);
  csv->set_header("utime", "x","y","z","quat_x","quat_y","quat_z","quat_w");

  // // Add cameras to persistent state
  flightGoggles->state.cameras.push_back(_renderCam);
  // Set environment
  flightGoggles->state.sceneFilename = environmentString;
}



// Read from the CSV
void OfflineRenderClient::updateCameraPose(Vector7d csvPose){

  // Get body pose in NED
  Transform3 bodyPose;
  bodyPose.fromPositionOrientationScale(Vector3(csvPose[0],csvPose[1],csvPose[2]),
    Quaternionx(csvPose[3],csvPose[4],csvPose[5],csvPose[6]), Vector3(1, 1, 1));

  // Get camera pose from body and body_T_camera transforms
  Transform3 cameraPose;
  cameraPose = envTransform * bodyPose * body_T_camera;

  // Populate status message with new pose
  flightGoggles->setCameraPoseUsingNEDCoordinates(cameraPose, 0);

}




////////////////////////////////////
// Example consumers and publishers
////////////////////////////////////

void imageConsumer(OfflineRenderClient& self){
    while (!(self.allFramesRequested && self.renderQueueLength<=0)){
      // Wait for render result (blocking).
      unity_incoming::RenderOutput_t renderOutput = self.flightGoggles->handleImageResponse();
      self.connected = true;

      if (renderOutput.renderMetadata.utime == 0){
        continue;
      }

      // Save render result
      for (int i = 0; i < renderOutput.images.size(); i++ ){
        fs::path filename;
        filename += self.renderDir;
        filename /= std::to_string(renderOutput.renderMetadata.utime) + std::string("_") + renderOutput.renderMetadata.cameraIDs[i] + std::string(".png");

		// If image file does not exist, write it to disk.
        // Compresses images on the fly if needed.
        if (!fs::exists(filename)){
        	cv::imwrite(filename, renderOutput.images[i]);
        }
      }

      // Update number of outstanding render requests.
      std::unique_lock<std::mutex> lk(self.mutexForRenderQueue); // temp lock
      self.renderQueueLength--;
      lk.unlock();
      self.renderQueueBelowCapacity.notify_all();
    }
}

void posePublisher(OfflineRenderClient& self){
  // Sends render requests to FlightGoggles from CSV
  int64_t utime;
  double x,y,z,quat_x,quat_y,quat_z,quat_w;

  while (self.csv->read_row(utime,x,y,z,quat_w,quat_x,quat_y,quat_z)){

      // Keep requesting frame while not connected
      while (!self.connected){
          self.flightGoggles->state.utime = 0;
          self.flightGoggles->requestRender(false);
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }



    // Wait for render queue to empty before requesting more frames
    // If blocked for more than 100ms, request a frame anyway
    std::unique_lock<std::mutex> lk(self.mutexForRenderQueue);
    self.renderQueueBelowCapacity.wait(lk, [&self]{return (self.renderQueueLength < self.renderQueueMaxLength);});

    while ((self.renderQueueLength < self.renderQueueMaxLength)){

        Vector7d csvPose;
        csvPose << x,y,z,quat_w,quat_x,quat_y,quat_z;

        // Update camera position
        self.updateCameraPose(csvPose);

        self.flightGoggles->state.utime = utime;

        // request render
        self.flightGoggles->requestRender(false);

        // Update render queue
        self.renderQueueLength++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    lk.unlock();
  }

  self.allFramesRequested = true;
}

// Master worker thread
void OfflineRenderClientThread(std::string environmentString, unity_outgoing::Camera_t _renderCam, int _instanceNum, std::string _trajectoryPath, Transform3 _cameraPoseOffset, Transform3 _envTransform, std::string _renderDir){

    // Create client object
    OfflineRenderClient worker(environmentString,_renderCam, _instanceNum, _trajectoryPath, _cameraPoseOffset, _envTransform, _renderDir);

    // Spawn worker threads
    // Fork sample render request thread
    // will request a simple circular trajectory
    std::thread posePublisherThread(posePublisher, std::ref(worker));

    // Fork a sample image consumer thread
    std::thread imageConsumerThread(imageConsumer, std::ref(worker));

    // Wait for threads to finish.
//    while (!(worker.allFramesRequested && worker.renderQueueLength <= 0)){
//        sleep(1000);
//    }

    posePublisherThread.join();
    imageConsumerThread.join();

}


// Calculate Unity transformation
Transform3 computeTransform(Vector7d offset){
    Transform3 body_T_cam;
    body_T_cam.fromPositionOrientationScale(
        Vector3(offset[0],offset[1],offset[2]),
        Quaternionx(offset[3],offset[4],offset[5],offset[6]),
        Vector3(1, 1, 1));
    return body_T_cam;
}


///////////////////////
// Example Client Node
///////////////////////

int main(int argc, char **argv) {

  // Get args
  if (argc != 8) {
    std::cerr << "Please provide args! ex: ./OfflineRenderClient Butterfly_World <num_cameras> <offset_x> <offset_y> <offset_z> <offset_z_deg> poseList.csv" << std::endl;
    std::cerr << "Note that offset arg is in NED." << std::endl;
    return 1;
  }
  std::string environment_string = argv[1];
  const int num_cameras = std::stoi(argv[2]);
  Vector7d environmentPoseOffset;
  Quaternionx q;
  q = Eigen::AngleAxisd(M_PI/180.0*std::stod(argv[6]), Eigen::Vector3d::UnitZ());
  environmentPoseOffset << std::stod(argv[3]),std::stod(argv[4]),std::stod(argv[5]),q.w(),q.x(),q.y(),q.z();

  std::cout << environmentPoseOffset << std::endl;

  Transform3 envTransform = computeTransform(environmentPoseOffset);

  //std::cout << envTransform << std::endl;

  std::string trajectoryPath(argv[7]);

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

  unity_outgoing::Camera_t camDepth;
  camDepth.ID = "Camera_Depth";
  camDepth.channels = 1;
  camDepth.isDepth = true;
  camDepth.outputIndex = 0;

  std::vector<unity_outgoing::Camera_t> cameras;

  // Save cameras
  cameras.push_back(camL);
  cameras.push_back(camR);
  cameras.push_back(camD);
  cameras.push_back(camDepth);

  // Define pose offsets for cameras
  std::vector<Transform3> cameraPoseTransforms;
  cameraPoseTransforms.push_back(computeTransform((Vector7d() << 0,-0.05,0,   1,0,0,0).finished()));
  cameraPoseTransforms.push_back(computeTransform((Vector7d() << 0,0.05,0,    1,0,0,0).finished()));
  cameraPoseTransforms.push_back(computeTransform((Vector7d() << 0,0,0,       0.707,0,-0.707,0).finished()));
   // Depth cam is same as left cam
  cameraPoseTransforms.push_back(computeTransform((Vector7d() << 0,-0.05,0,   1,0,0,0).finished()));




  // spawn parallel render clients for each camera
  std::vector<std::thread> workers;
  workers.reserve(num_cameras);

  for (int i = 0; i < num_cameras; i++){
      std::thread worker(OfflineRenderClientThread, environment_string, cameras[i], i, trajectoryPath, cameraPoseTransforms[i], envTransform, renderDir);

      workers.push_back(std::move(worker));

  }

  // Wait for worker threads to die
  for (auto& t : workers)
      t.join();

  return 0;
}
