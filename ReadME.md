# UAV Visual Localization & 3D Viewer

An interactive 3D visualization and navigation tool built in C++ using OpenGL, FreeGLUT, and OpenCV. This application correlates high-resolution satellite imagery with a sequence of UAV (drone) flight data, allowing users to inspect flight paths, view drone positions in a 3D environment, and project drone camera feeds onto their spatial coordinates.

## Demo

![Demo of the project](./Demo.mp4)

## Key Features

* **Geospatial Data Parsing:** Automatically loads and parses CSV datasets containing drone coordinates, altitude, timestamps, and 6-DOF pose parameters (Omega, Kappa, Phi).
* **Coordinate Mapping Transformation:** Converts real-world GPS coordinates (latitude and longitude) into normalized 3D OpenGL space relative to a regional satellite map boundary.
* **Interactive 3D Camera Modes:** * *Orbital View:* Smooth animated transitions to focus on specific drone waypoints, with mouse-driven rotation and zoom.
  * *Freecam Mode:* First-person navigation (`Z, Q, S, D` keys) with mouse look to explore the 3D space freely.
* **Dynamic Texture Loading:** Utilizes OpenCV to load, scale, and map high-resolution TIFF satellite textures and JPEG drone imagery in real-time.
* **Raycasting / Hover Detection:** Implements screen-to-world coordinate projection (`gluProject`) to detect and select individual drone nodes interactively.

## Prerequisites

To compile and run this project locally, you need:

1. **C++ Compiler:** A compiler supporting C++11 or higher (e.g., MinGW on Windows).
2. **Libraries:**
   * **OpenCV** (configured for your C++ environment)
   * **OpenGL & FreeGLUT** (for 3D rendering and window management)
3. **Dataset:** The [UAV-VisLoc](https://github.com/IntelliSensing/UAV-VisLoc) dataset structure (`UAV-VisLoc/`) must be placed in the root directory.

## Project Structure

* `main.cpp`: Core application handling the OpenGL rendering loop, camera matrices, user inputs, and dataset parsing.
* `Projet Drone.cbp`: Code::Blocks project file.
* `UAV-VisLoc/`: Dataset folder containing the satellite map, flight telemetry CSV, and drone frames.

## Installation & Compilation (Windows / MinGW)

1. Clone this repository:
```bash
git clone [https://github.com/lE6CANT68/uav-visual-localization-3d.git](https://github.com/lE6CANT68/uav-visual-localization-3d.git)