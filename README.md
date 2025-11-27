# Transport Simulator 3D


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