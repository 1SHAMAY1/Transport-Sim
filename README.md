# Transport Simulator 3D

A high-performance 3D transport simulation built from scratch using C++20 and OpenGL 4.6. This project demonstrates the application of hardcore Data Structures and Algorithms (DSA) in a real-time rendering environment, featuring a custom engine, graph-based road networks, and autonomous vehicle navigation using A* pathfinding.

## 🚀 Key Features

### Core Engine
- **Custom OpenGL Renderer**: Built from the ground up using modern OpenGL 4.6 (DSA - Direct State Access).
- **Batch Rendering**: Optimized rendering pipeline that batches static geometry (nodes, roads) and dynamic elements (traffic lights) to minimize draw calls and maximize FPS.
- **Camera System**: Flexible 3D camera with both automatic orbiting and manual free-flight modes.

### Simulation Logic
- **Graph Data Structure**: The road network is represented as a directed graph using adjacency lists, allowing for efficient traversal and connectivity checks.
- **A* Pathfinding**: Autonomous vehicles calculate optimal routes in real-time using the A* algorithm with a priority queue.
- **Traffic Systems**:
    - **Traffic Lights**: Dynamic traffic light system with state machines (Red, Yellow, Green) controlling intersection flow.
    - **Vehicle AI**: Vehicles obey traffic laws, stop at red lights, and navigate complex intersections.
    - **Scale**: Simulates **100 autonomous vehicles** with constant respawning to maintain traffic density.

### Visualization
- **Real-time Stats**: Integrated ImGui dashboard showing network statistics (nodes, edges), vehicle counts, and performance metrics (FPS, Frame Time).
- **Dynamic Lighting**: Visual representation of traffic light states and vehicle colors.

## 🎮 Controls

| Key | Action |
| :--- | :--- |
| **SPACE** | Toggle between Auto-Orbit and Manual Camera Mode |
| **W / S** | Move Camera Forward / Backward |
| **A / D** | Move Camera Left / Right |
| **Q / E** | Move Camera Up / Down |
| **Arrow Keys** | Rotate Camera (Pitch / Yaw) |

## 🛠️ Technology Stack

- **Language**: C++20
- **Graphics API**: OpenGL 4.6
- **Windowing**: GLFW
- **Math**: GLM (OpenGL Mathematics)
- **UI**: Dear ImGui
- **Build System**: Premake5 & MSBuild

## 🏗️ Architecture

The project follows a modular architecture separating the core engine, rendering, and simulation logic:

```
src/
├── Core/
│   ├── Application.cpp   # Main loop, window management, input handling
│   └── Application.h
├── Renderer/
│   ├── Camera.cpp        # 3D Camera implementation
│   ├── Shader.cpp        # GLSL Shader management
│   └── ...
├── Simulation/
│   ├── TransportSimulation.cpp # Simulation manager
│   ├── Graph.cpp         # Graph data structure for road network
│   ├── Pathfinding.cpp   # A* algorithm implementation
│   ├── Vehicle.cpp       # Vehicle entity and AI

1.  **Generate Project Files**:
    Run the `GenerateProjectFiles.bat` script to create the Visual Studio solution using Premake.
    ```batch
    .\GenerateProjectFiles.bat
    ```

2.  **Build the Project**:
    Open `Transport-Sim.sln` in Visual Studio or run the build script:
    ```batch
    .\Build.bat
    ```

3.  **Run**:
    The executable will be located in the `bin/Debug` (or `bin/Release`) directory.
    ```batch
    .\bin\Debug\Transport-Sim.exe
    ```