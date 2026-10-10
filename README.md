# 🏊 Infinite Swimmer

A 3D endless swimming game developed using a custom C++ OpenGL engine and controlled through a Python-powered webcam and voice tracking system.

---

# 🎓 Project Information

**Developer:** Ahmed AlHennawi

**Program:** MSc Computer Science – University of the Arts London (UAL)

**Repository:** https://git.arts.ac.uk/a-alhennawi0520251/swimmerProject

---

# 📖 Overview

**Infinite Swimmer** is an immersive endless-runner-style swimming game that combines real-time computer vision, voice recognition, and custom 3D graphics programming.

Players control a 3D dolphin using natural hand gestures detected through a webcam or voice commands processed by a Python controller. As survival time increases, the game's difficulty progressively scales, creating an engaging and increasingly challenging experience.

The project demonstrates the integration of:

* Modern OpenGL graphics programming
* Custom 3D object (.obj) parsing and rendering
* Object-Oriented game architecture in C++
* Computer vision using MediaPipe
* Voice recognition using Vosk
* UDP networking
* Real-time audio management
* Persistent data storage

---

# ✨ Features

## 🎮 Multimodal Player Controls

Control the dolphin using either hand gestures or voice commands.

### Hand Tracking

* Move your hand left or right to steer.
* Make a fist gesture to dive beneath obstacles.

### Voice Commands

Supported commands include:

* "Left"
* "Right"
* "Dive"

The system combines gesture recognition and speech recognition to provide an accessible and interactive gameplay experience.

## Controls

| Input Method | Action |
|-------------|---------|
| Move hand left | Swim left |
| Move hand right | Swim right |
| Fist gesture | Dive |
| Voice: Left | Swim left |
| Voice: Right | Swim right |
| Voice: Dive | Dive |

---
## Screenshots

### Main Gameplay

![Gameplay](screenshots/gameplay.png)

![Gameplay](screenshots/gameplay1.png)

### Hand Tracking Control

![Hand Tracking](screenshots/hand_tracking.png)

---

## 🌊 Custom 3D Rendering Engine

Built entirely using OpenGL and C++, featuring a modular architecture and advanced graphics techniques:

### Custom 3D Model Loader

A bespoke `.obj` parser capable of:

* Triangulating polygonal faces
* Automatically centering imported models
* Dynamically scaling meshes for rendering

### Real-Time Lighting

Fragment shaders utilize derivative functions (`dFdx`, `dFdy`) to generate dynamic normals and calculate diffuse lighting in real time.

### Procedural Animation

Sine-wave-driven vertex transformations create natural swimming motions, including:

* Tail wagging
* Body pitching

### Advanced Particle System

Configurable particle generation simulates:

* Water splashes
* Directional swimming wakes

### Environmental Effects

* Transparent water layers
* Infinite scrolling floor textures
* Depth-based fog rendering

---

## 📈 Dynamic Difficulty System

As the player survives longer:

* Swimming speed increases
* Floor scrolling accelerates
* Obstacle spawn frequency rises
* Overall challenge scales automatically

---

## 🏆 Persistent High Scores

The game automatically:

* Saves high scores to `highscore.txt`
* Loads previous records on startup
* Maintains player progress between sessions

---

# 🏗️ System Architecture

## Architecture Overview

```text
Webcam
   │
   ▼
MediaPipe
   │
   ▼
Python Controller
   │ UDP
   ▼
OpenGL Engine
   │
   ▼
Swimmer Movement



Microphone
   │
   ▼
Vosk
   │
   ▼
Python Controller
```

---

## C++ Game Engine

The engine was extensively refactored from a monolithic implementation into a clean, modular, Object-Oriented architecture.

### Core Components

**main.cpp**

The primary entry point responsible for:

* GLFW window creation
* OpenGL context initialization
* Main game loop execution

**GameEngine.h**

Central game-state manager handling:

* Game logic
* Score tracking
* Time progression
* Update orchestration

**Renderer.h**

Responsible for:

* OpenGL draw calls
* Model transformations
* Lighting calculations

**Model.h**

Loads and parses complex `.obj` meshes, including the player dolphin model.

**Shader.h**

Provides abstraction for:

* Shader compilation
* Shader linking
* Uniform management

**ParticleSystem.h**

Manages:

* Particle creation
* Physics updates
* Lifecycle handling

**ObstacleManager.h**

Handles:

* Obstacle spawning
* Collision detection

**UIManager.h**

Responsible for rendering the ImGui-based user interface.

**AudioManager.h**

Manages:

* Sound effects
* Background music
* Audio playback through miniaudio

**NetworkManager.h**

Provides asynchronous UDP communication with the Python controller.

---

## Python Controller (controller.py)

Responsible for:

* Webcam capture
* Hand tracking via MediaPipe
* Gesture recognition (open hand vs. fist)
* Positional smoothing
* Dead-zone calculation
* Voice command recognition using Vosk
* Sending control data to the game engine

---

# 🎤 Voice Recognition Setup

This project uses the Vosk speech recognition engine.

### 1. Download the English Vosk Model

Download:

```text
vosk-model-small-en-us-0.15
```

### 2. Extract the Model

Extract the downloaded archive and rename the folder to:

```text
model
```

### 3. Place the Model in the Project Root

```text
swimmerProject/
├── controller.py
├── model/
└── ...
```

---

# 📂 Project Structure

```text
swimmerProject/
│
├── assets/
│   ├── box.jpg
│   ├── tiles.jpg
│   ├── water.png
│   ├── crash.mp3
│   ├── dive.mp3
│   ├── swim.mp3
│   └── dolphin.obj
│
├── build/
│   ├── CMake build environment
│   └── swimmerProject executable
│
├── dependencies/
│   ├── GLFW
│   ├── GLAD
│   ├── ImGui
│   ├── miniaudio
│   └── stb_image
│
├── venv/
│   └── Python virtual environment
│
├── controller.py
├── model/
├── main.cpp
├── GameEngine.h
├── Renderer.h
├── Model.h
├── Shader.h
├── ParticleSystem.h
├── ObstacleManager.h
├── UIManager.h
├── AudioManager.h
├── NetworkManager.h
└── CMakeLists.txt
```

---

# 🔧 Technologies Used

## C++

* OpenGL
* GLFW
* GLAD
* ImGui
* miniaudio
* stb_image
* GLM

## Python

* MediaPipe (Hand Tracking)
* OpenCV (Image Processing)
* Vosk (Offline Speech Recognition)
* PyAudio (Microphone Stream Processing)

---

# ⚙️ Technical Challenges

### Custom 3D Geometry Processing

Designed and implemented a custom `.obj` parser capable of:

* Triangulating N-gons and quad faces
* Automatically normalizing model scales
* Dynamically centering imported meshes

### Graphics Programming

Implemented procedural animation and real-time lighting effects directly within OpenGL shaders without relying on external graphics frameworks.

### Software Architecture

Refactored a monolithic application into a scalable and maintainable Object-Oriented architecture.

### Real-Time Integration

Successfully synchronized:

* Computer vision processing
* Speech recognition
* Network communication
* Real-time rendering

### Gesture Stabilization

Improved controller reliability through positional smoothing and dead-zone filtering.

---

# 🚀 Build and Run

## 1. Compile the C++ Engine

```bash
cd build
cmake ..
make
```

## 2. Start the Python Controller

Open a new terminal:

```bash
source venv/bin/activate
python controller.py
```

## 3. Launch the Game

```bash
./build/swimmerProject
```

---

# 🎯 Learning Outcomes

This project demonstrates practical experience in:

* Object-Oriented C++ software architecture
* Low-level 3D graphics programming
* Shader development and rendering pipelines
* 3D geometry parsing and mathematical transformations
* Human-computer interaction through multimodal input systems
* Computer vision applications
* UDP network communication
* Cross-language system integration between C++ and Python

---

# Future Improvements

- Full body pose tracking
- Multiplayer support
- Advanced water shaders
- Additional voice commands
- Mobile deployment
  
---

# 📜 License

This project was developed for academic and educational purposes as part of the MSc Computer Science programme at the University of the Arts London (UAL).
