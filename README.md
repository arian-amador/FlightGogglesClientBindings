# FlightGoggles Client Bindings

Client bindings for the [FlightGoggles hardware in-the-loop simulation environment](https://github.com/AgileDrones/FlightGoggles).

Quick start instructions can be found [here](https://github.com/AgileDrones/FlightGoggles/README.md) 

## Repo Format

```bash
├── build
│   └─── bin            # Client executables will be placed here. 
│
├── CMakeLists.txt      # Top level compilation flags. Enable ROS
│                       # > binding compilation here.
├── README.md
├── launch              # ROS launch files
│   └── flightGogglesClient.launch
├── package.xml         # ROS package.xml
├── README.md
└── src
    ├── CMakeLists.txt
    ├── Common                      # Low level client code for FlightGoggles
    │   ├── CMakeLists.txt
    │   ├── FlightGogglesClient.cpp # Main client library.
    │   ├── FlightGogglesClient.hpp
    │   ├── json.hpp                # External json parsing library.
    │   ├── jsonMessageSpec.hpp     # FlightGoggles message API spec. Check here 
    │   │                           # > to see what settings are available. 
    │   └── transforms.hpp          # Handles transformations from ROS-like coordinates
    │                               # > to Unity3D coordinates.
    ├── GeneralClient               # A simple example client that publishes 
    │   ├── CMakeLists.txt          # > and subscribes to FlightGoggles images
    │   ├── GeneralClient.cpp       # > using OpenCV bindings.
    │   └── GeneralClient.hpp
    └── ROSClient                   # Beta ROS-aware extension of the base FlightGoggles client
        ├── CMakeLists.txt          # > that subscribes to poses and outputs images over ROS.
        ├── ROSClient.cpp           # > NOT compiled by default. Edit CMakeLists.txt to enable.
        └── ROSClient.hpp
```
## Citation
If you find this work useful for your research, please cite:
```bibtex
@inproceedings{sayremccord2018visual,
  title={Visual-inertial navigation algorithm development using photorealistic camera simulation in the loop},
  author={Sayre-McCord, Thomas and
  Guerra, Winter and
  Antonini, Amado and
  Arneberg, Jasper and
  Brown, Austin and
  Cavalheiro, Guilherme and
  Fang, Yajun and
  Gorodetsky, Alex and
  McCoy, Dave and
  Quilter, Sebastian and
  Riether, Fabian and
  Tal, Ezra and
  Terzioglu, Yunus and
  Carlone, Luca and
  Karaman, Sertac},
  booktitle={2018 IEEE International Conference on Robotics and Automation (ICRA)},
  year={2018}
}
```
