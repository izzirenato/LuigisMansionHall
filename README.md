# Luigi's Mansion Hall

This was my first experience working with both OpenGL and C++, developed as a group project focused on computer graphics and real-time rendering techniques.

## Project Overview

This project demonstrates various computer graphics concepts through the recreation of Luigi's Mansion's atmospheric hall environment. As my introduction to both C++ and OpenGL development, it explores fundamental graphics programming concepts including lighting models, shader programming, and 3D scene management.

## Technical Features

- **Phong Lighting Model**: Multiple light sources including directional and point lights
- **Custom Shader System**: GLSL vertex and fragment shaders for materials and effects
- **3D Scene Management**: Detailed hall architecture with walls, wooden elements, metal fixtures, and decorative objects
- **Atmospheric Effects**: Real-time lighting and visual effects for immersive ambiance
- **Interactive Camera System**: First-person navigation with mouse controls

## Technologies Used

- **C++** - Core application development (first project)
- **OpenGL 3.3+** - Graphics rendering API (first experience)
- **GLFW** - Window management and input handling
- **Assimp** - 3D model loading and processing
- **Custom Math Library** - Vectors, matrices, quaternions, and camera utilities

## Project Structure

```
├── src/
│   ├── core/           # Main application and mathematical utilities  
│   ├── managers/       # Scene and resource management systems
│   ├── rendering/      # OpenGL rendering pipeline and shaders
│   └── effects/        # Visual effects implementation
├── assets/
│   ├── models/         # 3D models and geometry (.obj files)
│   ├── shaders/        # GLSL shader programs
│   └── textures/       # Texture assets and materials
└── third_party/        # External libraries (GLFW, Assimp, GLAD)
```

## Development Notes

**Group Project** - My primary contributions:
- Core application development and architecture
- Shader programming and graphics effects implementation
- Mathematical utilities and rendering pipeline development

This project served as my introduction to graphics programming, representing my first hands-on experience with both C++ development and OpenGL rendering techniques.

Built with Visual Studio and requires OpenGL-compatible graphics hardware.

## Build and Run

Requirements: Windows, Visual Studio 2022 with the *Desktop development with C++* workload, and an OpenGL 3.3+ capable GPU.

1. Open `LuigisMansionHall.sln` in Visual Studio.
2. Select the **Debug** and **x64** configuration.
3. Press **F5** (or **Ctrl+F5** to run without the debugger).

The GLFW and Assimp libraries are included in `third_party/lib/windows/x64`. A post-build step copies `assimp-vc143-mt.dll` next to the executable, so no manual setup is needed. Run the program from Visual Studio, or with the project folder as working directory, because shaders, models and `config.xml` are loaded through relative paths.

Controls: WASD to move, Space/Shift for up/down, P to pause, ESC to exit.

## License

MIT License - see [LICENSE](LICENSE) file for details.