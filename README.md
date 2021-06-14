# LORE - Lightweight Object-oriented Rendering Engine

### NOTE: **I do not condone object-oriented programming as The Way™ of developing software. This project was partially an experiment in OOP.**

### I recommend [this video](https://www.youtube.com/watch?v=QM1iUe6IofM) on the topic of OOP.
<br/>

### **[Jump to Building](#building)**
### **[Jump to Architecture](#architecture)**

## What is LORE?

![Lore1](images/lore1.gif)
![Lore2](images/lore2.gif)
![Lore3](images/lore3.gif)
![Lore4](images/lore4.gif)
![Lore5](images/lore5.gif)
![Lore6](images/lore6.gif)

LORE is a 2D and 3D rendering engine intended to provide an abstraction layer on graphics APIs, while also providing built-in shaders, rendering algorithms, and scene management, to name a few. I started this project to learn more about graphics, modern C++, and software architecture. It is also intended to be lightweight with minimal dependencies (hence the name).

### Engine Features
- Scene graph
- Dynamically loaded render API plugins
- Resource file management (parsing, indexing)
- Robust material/shading pipeline (easy to create custom effects)
- Custom memory management (memory pooling)
- Uber shader generation at runtime
- Data-driven configuration and scene loading
- In-engine CLI

### Graphics Features
- Realtime lighting (Blinn-Phong)
- Render to texture
- Normal mapping
- Shadows (directional and omnidirectional)
- Post processing (HDR, bloom)
- Instancing
- Anti-aliasing
- Transparency
- UI (powered by ImGui)
- Model loading (with help from Assimp)
- Forward 2D/3D renderer

### Feature Wishlist (Basic)
- Deferred renderer
- Reflection probes
- Frustum culling
- Scene editing in-engine
- Gradient background/skybox
- SSAO
- Grid

### Feature Wishlist (Big Dreams)
- Vulkan and D3D12 render plugins
- PBR
- Better shadows (fix peter-panning, flickering)

## Using the Engine
The main idea of setting up rendering in LORE is using a `RenderView`, which sets up a scene, camera, and viewport dimensions. By using two of these, it's very easy to setup an offscreen scene and sample it in the main scene (as seen in the 3D sample, where the 2D sample is rendered inside it).

The provided samples showcase how to setup the main LORE objects (The `Context` - the object that manages everything, a `Scene`, and a `Camera`, etc.),  how to load resources, and render the scene.

## Building

### Clone and initialize submodules
`git clone https://github.com/MemoryDealer/LORE.git`

`cd LORE`

`git submodule update --init --recursive`

### Generate the LORE project files and build
Navigate to `src` and run `generate_project.bat` to generate the Visual Studio project files using Premake.

Open `LORE.sln` and build the solution (you probably want the release build for good performance).

You are done :)

### Running the samples

By default the `Sample3D` project is selected. There is also the `Sample2D` project, and the unit tests.

## Tips and Tricks

### Debug UI
Press shift and `~` to bring up the debug UI and console. Alternatively you can only display the performance stats with `shift + ~`. There are just a few options in the debug UI currently.

![Lore1](images/lore-ui.gif)

#### Some Console Commands (not case sensitive):
- `SetNodePos [node name] [x],[y],[z]`: sets the position of a node in world space
- `MoveNode [node name] [x],[y],[z]`: translates node in world space
- `SetLightColor [node name] [r],[g],[b]`: changes a light's color

Scene node names for `Sample3D` can be found in `LORE\res\Sample3D\Sample3D.scene` and in the `Game.cpp` files for `Sample2D`.

E.g.,
```
"Layout": {
    "Nodes": {
      "MagicSphere": {
        "Prefab": "MagicSphere",
        "Position": [0.0, 3.0, 0.0],
        "ChildNodes": {
          "RedCube": {
            "Prefab": "GlowingRedCube",
            "Light": "LightRed",
            "Scale": 0.25
          },
```
"MagicSphere" and "RedCube" are a couple of node names.

### 2D Sample Controls 
- Move with standard WASD
- Z/X to zoom camera in/out

### 3D Sample Controls
- Move camera with standard WASD
- Mouse to rotate camera
- Q/E to move up/down in world space
- Shift to speed up movement
- Arrow keys move the player in the 2D scene

## Bonus Image(s)

2D scene with light colors changed (e.g., `setlightcolor torch0 [r],[g],[b]`):
![2D Sample](images/2Dscene.png)

<br/>

# Architecture
**WIP ... pretty charts and stuff coming soon**
