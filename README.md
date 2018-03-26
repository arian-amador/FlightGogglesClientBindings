# FlightGoggles Client Bindings

Client bindings for the [FlightGoggles hardware in-the-loop simulation environment](https://github.com/AgileDrones/FlightGoggles).

Quick start instructions can be found [here](https://github.com/AgileDrones/FlightGoggles/README.md) 

# Quick Start Guide for OpenCV Bindings

```bash
# Install required libraries
sudo apt install libzmqpp-dev libeigen3-dev libopencv-dev
# Clone client bindings
git clone --recursive https://github.com/AgileDrones/FlightGogglesClientBindings.git
# Setup and build
cd FlightGogglesClientBindings
mkdir build
cd build
cmake ../ && make
```

# Repo Format

```bash
├── build
│   └─── bin            # Client executables will be placed here. 
│
├── CMakeLists.txt      # Top level compilation flags. Enable ROS
│                       # binding compilation here.
├── README.md
└── src
    ├── CMakeLists.txt  
    ├── Common          # Low level client code for FlightGoggles
    │
    ├── GeneralClient   # A simple example client that publishes 
    │                   # and subscribes to FlightGoggles images
    │                   # using OpenCV bindings.
    │
    └── ROSClient       # ROSified extension of the base FlightGoggles
                        # client. Not compiled by default. Edit 
                        # CMakeLists.txt to enable.
```