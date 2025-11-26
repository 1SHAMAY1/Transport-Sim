# Transport Simulator 3D

A 3D transport simulation built with C++ and hardcore DSA (Data Structures and Algorithms). Features a custom OpenGL rendering engine, graph-based road networks, and A* pathfinding for autonomous vehicle navigation.

## Features
- **Custom OpenGL Engine**: Built from scratch with C++20 and OpenGL 4.6
- **Graph Data Structure**: Adjacency list representation for road networks
- **A* Pathfinding**: Optimal route finding using priority queue
- **3D Rendering**: Camera system, shader pipeline, geometric rendering
- **Real-time Simulation**: Autonomous vehicles navigating city grids

## Build
```batch
.\GenerateProjectFiles.bat
.\Build.bat
.\bin\Debug\Transport-Sim.exe
```

## Technologies
- C++20
- OpenGL 4.6
- GLFW (windowing)
- GLM (mathematics)
- Premake5 (build system)

## Architecture
```
src/
├── Core/         - Application loop, window management
├── Renderer/     - Camera, shaders, OpenGL abstractions  
├── Simulation/   - Graph, pathfinding, vehicles, simulation manager
└── main.cpp
```

See [walkthrough.md](file:///C:/Users/shank/.gemini/antigravity/brain/19696a85-6300-4ddc-8920-db8d3972b3be/walkthrough.md) for detailed documentation.