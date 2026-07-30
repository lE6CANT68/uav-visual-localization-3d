# UAV Visual Localization & 3D Viewer

An interactive **3D visualization and navigation tool** built in **C++** using **OpenGL**, **FreeGLUT**, and **OpenCV**.

This application correlates high-resolution satellite imagery with a sequence of UAV (drone) flight data, allowing users to inspect flight paths, visualize drone positions in a 3D environment, and project drone camera feeds onto their corresponding spatial coordinates.

---

## Demos

### Orbital View

![Orbital View Demo](./demo 1.gif)

### Freecam View

![Freecam View Demo](./demo 2.gif)

---

## Key Features

* **Geospatial Data Parsing**

  * Automatically loads and parses CSV datasets containing drone coordinates, altitude, timestamps, and 6-DOF pose parameters (Omega, Kappa, Phi).

* **Coordinate Mapping Transformation**

  * Converts real-world GPS coordinates (latitude and longitude) into normalized OpenGL 3D space relative to the satellite map boundaries.

* **Interactive 3D Camera Modes**

  * **Orbital View**

    * Smooth camera animations focused on selected drone waypoints.
    * Mouse-controlled rotation and zoom.
  * **Freecam Mode**

    * First-person navigation using `Z`, `Q`, `S`, `D`.
    * Mouse look for exploring the 3D scene.

* **Dynamic Texture Loading**

  * Uses OpenCV to load and render high-resolution TIFF satellite imagery and JPEG drone frames in real time.

* **Raycasting / Hover Detection**

  * Implements screen-to-world projection (`gluProject`) to detect and interactively select drone waypoints.

---

## Prerequisites

Before building the project, make sure you have:

* A **C++11** (or newer) compatible compiler (MinGW recommended on Windows).
* **OpenGL**
* **FreeGLUT**
* **OpenCV 4.x**

### Dataset

The following dataset structure must be placed at the project root:

```text
UAV-VisLoc/
├── satellite03.tif
├── 03.csv
└── drone/
```

A sample dataset is available from the official **UAV-VisLoc** repository:

https://github.com/IntelliSensing/UAV-VisLoc

---

## Project Structure

```text
.
├── main.cpp                # Main application
├── Projet Drone.cbp        # Code::Blocks project
└── UAV-VisLoc/
    ├── satellite03.tif
    ├── 03.csv
    └── drone/
```

---

## Installation (Windows / MinGW)

### 1. Clone the repository

```bash
git clone https://github.com/lE6CANT68/uav-visual-localization-3d.git
cd uav-visual-localization-3d
```

### 2. Open the project

Open `Projet Drone.cbp` using **Code::Blocks**.

### 3. Configure the libraries

Configure your build options to include:

#### Link directories

* OpenCV
* FreeGLUT

#### Link libraries

* `opengl32`
* `freeglut`
* `opencv_core`
* `opencv_imgcodecs`
* `opencv_imgproc`
* `opencv_highgui`

(and any additional OpenCV modules required by your installation)

### 4. Copy runtime files

Place the following next to the generated executable (`bin/Debug` or `bin/Release`):

* the `UAV-VisLoc/` folder
* the required OpenCV DLLs
* the FreeGLUT DLL

### 5. Build & Run

Compile and launch the project from **Code::Blocks**.

---

## Controls

| Action             | Control      |
| ------------------ | ------------ |
| Toggle camera mode | `1`          |
| Move forward       | `Z`          |
| Move backward      | `S`          |
| Move left          | `Q`          |
| Move right         | `D`          |
| Move up            | `Space`      |
| Move down          | `Left Shift` |
| Look around        | Mouse        |
| Select waypoint    | Left Click   |
| Orbital zoom       | Mouse Wheel  |
| Freecam opacity    | Mouse Wheel  |
| Adjust global yaw  | `O` / `P`    |
| Exit               | `ESC`        |

---

## Technologies

* C++
* OpenGL
* FreeGLUT
* OpenCV
* Code::Blocks
* CSV Parsing
* Texture Mapping
* Raycasting
* GPS Coordinate Transformation
* 3D Camera Navigation

---

## Disclaimer

This project was developed for **educational and portfolio purposes** using sample data from the **UAV-VisLoc** research dataset.

Dataset credit goes to the original **UAV-VisLoc** authors and contributors.
