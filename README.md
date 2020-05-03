# LORE - Lightweight Object-oriented Rendering Engine

## Build Instructions (Windows)

### Initialize submodules (from the root directory):

`git submodule update --init --recursive`

### Build assimp, first you will need to install CMake and run the following commands:

`cd src/External/assimp`

`cmake CMakeLists.txt`

Then build the project "assimp" inside the Assimp Visual Studio solution or by command line.

### Generate the LORE project files
We can go up one directory and run `make.bat` to generate the Visual Studio project files using Premake.

Open the solution (you may have to upgrade if using VS 2019), and build all.

You are done :)

### Running the samples

By default the `Demo3D` project is selected. There are also two other projects: `Example_ComplexScene2D` and `Example_ComplexScene3D`.

## Tips and Tricks

### Debug Console
Press shift and ~ to bring up the debug console (also brings up the FPS display which can be toggled alternatively with ~).

#### Some Commands
- `SetNodePos [node name] [x] [y]`: sets the position of a node in world space
- `MoveNode [node name] [x] [y] [z]`: translates node in world space
- `SetLightColor [node name] [r] [g] [b]`: changes a light's color
- `Reload`: reloads the scene (if loaded from file)

Scene node names for `Demo3D` can be found in `LORE\res\Demo3D\Demo3D.scene` and in the `Game.cpp` files for the complex scene projects.

E.g.,
```
  "Layout": {
    "Nodes": {
      "FloatingLight": {
        "Entity": "Light",
        "Light": "PointLight0",
        "Position": [0.0, 3.5, 0.0],
        "Scale": 0.25
      },
      "Star": {
        "Entity": "Star",
        "Position": [0.0, 0.0, -8.0],
        "Scale": 0.2
      }
    }
  }
```
