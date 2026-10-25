# Physics Engine

A 2D gravity and collision simulation written in C++, rendered in real time with OpenGL. Circles fall under gravity and bounce around inside a container.

<!-- Add a screen-recorded GIF of the simulation here, for example: -->
<!-- ![Demo](docs/demo.gif) -->

## Features

- Circles that fall under gravity
- A container the circles collide with and bounce off, with energy lost on each bounce (restitution)
- Floor friction
- Fixed 60 Hz simulation timestep
- Real-time rendering with core-profile OpenGL 3.3
- Orthographic projection that keeps circles round when the window is resized
- Circles spawned from the center in a rotating radial pattern to demonstrate the simulation

Circles do not collide with each other yet. See the [Roadmap](#roadmap).

## Tech Stack

- **Language:** C++
- **Graphics:** OpenGL 3.3 (core profile), loaded with [GLAD](https://github.com/Dav1dde/glad)
- **Windowing and input:** [GLFW](https://www.glfw.org/)
- **Math:** [GLM](https://github.com/g-truc/glm)
- **Build system:** CMake with Ninja
- **Dependency management:** vcpkg (manifest mode), which installs GLFW and GLM automatically. GLAD is included in `src/`.

## Getting Started

### Prerequisites

- A C++ compiler with C++17 support (MSVC, GCC, or Clang)
- [CMake](https://cmake.org/) 3.20 or newer (required by the presets file)
- [Ninja](https://ninja-build.org/)
- [vcpkg](https://github.com/microsoft/vcpkg), with the `VCPKG_ROOT` environment variable set to your vcpkg folder
- A GPU and driver that support OpenGL 3.3

### Build and Run

```bash
git clone https://github.com/ethanGultomm/Physics-Engine.git
cd Physics-Engine
```

Set `VCPKG_ROOT` to the folder where vcpkg is installed:

```bash
# Linux / macOS
export VCPKG_ROOT=/path/to/vcpkg

# Windows (PowerShell)
$env:VCPKG_ROOT = "C:\path\to\vcpkg"
```

Then configure and build with the `vcpkg` preset. vcpkg reads `vcpkg.json` and installs GLFW and GLM for you the first time.

```bash
cmake --preset vcpkg
cmake --build build
```

The build output goes in the `build/` folder. Run the generated executable.

```bash
.\build\2Dsim.exe
```

### Controls

| Key   | Action           |
|-------|------------------|
| `Esc` | Close the window |

## How It Works

The simulation and the rendering are kept separate.

- **`World`** owns a list of rigid bodies. Each frame it applies the border constraints, then updates every body, then renders them.
- **`RigidBody`** holds a position, velocity, mass, restitution, and a polymorphic `Shape`.
- **`Shape` / `Circle`** know how to draw themselves. A circle is drawn as a triangle fan (50 segments by default) from its own vertex array and buffers, and frees its OpenGL resources in its destructor.

### Each simulation step

1. Gravity is added to each body's velocity.
2. Each body moves by its velocity (semi-implicit Euler integration).
3. Bodies that cross a wall are pushed back inside the container and their velocity is reflected, scaled by the restitution (0.7) so every bounce loses energy. Floor contact also applies friction.

The step runs at a fixed 60 Hz.

## Project Structure

```
.
├── src/
│   ├── GravitySim.cpp   # Entry point: window, OpenGL context, main loop
│   ├── world.cpp/.h     # Owns bodies, steps the simulation, renders
│   ├── circle.cpp/.h    # Circle shape: mesh generation, shaders, drawing
│   ├── rigidBody.h      # Position, velocity, mass, restitution, shape
│   ├── shape.h          # Shape interface
│   └── glad.c           # OpenGL loader
├── CMakeLists.txt
├── CMakePresets.json    # "vcpkg" preset (Ninja, build/ output folder)
└── vcpkg.json           # Dependencies: glfw3, glm
```
