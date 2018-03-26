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
