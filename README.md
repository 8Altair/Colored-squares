# Colored Squares

## Overview
**Colored Squares** is a cross-platform Qt 6 demonstration app that illustrates how to integrate modern **OpenGL (4.6 Core Profile)** into a **Qt Widgets** application.  
The program opens a main window, requests an OpenGL 4.6 core context, and uses a custom `QOpenGLWidget` subclass (`View`) to render a simple 2D scene — a ground strip and a **pyramid of brightly coloured squares** stacked in layers.

---

## Features
- Uses **OpenGL 4.6 Core Profile** (no deprecated fixed-pipeline functions)
- Draws a **pyramid of colored squares** using modern buffer/shader APIs
- Includes an **orthographic projection** that adapts to window resizing
- Automatically sets up **Qt application icons** for window, taskbar, and executable
- Built with **CMake**, **GLM**, and **Qt 6**

---

## Prerequisites
Before building, ensure the following dependencies are installed:

| Dependency | Minimum Version | Purpose |
|-------------|-----------------|----------|
| **Qt 6** | 6.5 or newer | Core / Gui / Widgets / OpenGL / OpenGLWidgets |
| **CMake** | 3.21 or newer | Build system |
| **GLM** | Latest | Math library for transformations |
| **MinGW** | Qt-provided toolchain | Compiler |

---

## How It Works

### Rendering
- A **unit square** is defined as two triangles stored in a **Vertex Buffer Object (VBO)**.
- A **Vertex Array Object (VAO)** is used to describe how vertex data is interpreted.
- A minimal **GLSL shader program** transforms each vertex using a **Model–View–Projection (MVP)** matrix and colors it with a uniform RGBA value.
- Multiple squares are drawn at different positions and with different colors to form a pyramid:
  - **Base row:** four colored squares (red, green, blue, orange)  
  - **Second row:** three squares (violet, yellow, turquoise)  
  - **Top row:** one pink square
- A gray “ground strip” is drawn beneath the pyramid for depth reference.

### OpenGL Setup
- The application requests a **Core Profile OpenGL 4.6 context** using `QSurfaceFormat`.
- The context is initialized in `View::initializeGL()` where shaders and buffers are created.
- Each frame is rendered inside `paintGL()`, which:
  1. Clears the framebuffer with a dark gray-blue color.
  2. Binds the shader program and VAO.
  3. Uploads new transformation matrices and color uniforms.
  4. Issues draw calls with `glDrawArrays(GL_TRIANGLES)` for each square.
- The projection is **orthographic** and recalculated on every window resize in `resizeGL()` to maintain proper aspect ratio.

### Shader Pipeline
- **Vertex Shader (`#version 450 core`):**
  ```glsl
  layout(location = 0) in vec3 position;
  uniform mat4 mvp;
  void main()
  {
      gl_Position = mvp * vec4(position, 1.0);
  }
  ```

- **Fragment Shader (`#version 450 core`):**
  ```glsl
  uniform vec4 color;
  out vec4 FragColor;
  void main()
  {
      FragColor = color;
  }
  ```

- These shaders are compiled, linked, and cached during initialization.

### Icon System
- The app includes two icons inside `Icon/`:
  - `Icon/Icon.png` — used for window and taskbar icons.
  - `Icon/icon.ico` — embedded in the executable for Windows Explorer.
- **Qt Resource System (`resources.qrc`)** embeds both files into the binary.
- **Windows Resource Script (`appicon.rc`)** ensures the `.exe` displays the correct icon in file explorer.
- Both resources are added in `CMakeLists.txt` under the `add_executable()` target.

### CMake & Build Integration
- CMake enables Qt’s automatic tools:
  ```cmake
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)
  set(CMAKE_AUTOUIC ON)
  ```
- GLM is fetched automatically via:
  ```cmake
  FetchContent_Declare(
      glm
      GIT_REPOSITORY https://github.com/g-truc/glm.git
      GIT_TAG master)
  FetchContent_MakeAvailable(glm)
  ```
- On Windows, post-build commands copy the required **Qt DLLs** and **platform plugins** into the build directory, so the executable can run standalone.

### Deployment Notes
To ensure full runtime compatibility, you can also use:
```bash
windeployqt --compiler-runtime --no-translations build/Colored_squares.exe
```
This command bundles all required Qt and MinGW runtime libraries for distribution.

### Customization Tips
- Change colors inside `View::paintGL()` where each square’s RGBA values are set.
- Adjust the number and spacing of squares for different arrangements.
- Replace the icons in `Icon/` to rebrand the application.
- Modify `CMakeLists.txt` if you add new source files or Qt modules.

---

## License
This project is provided for **educational and demonstration purposes**.  
You are free to reuse, modify, or extend it in your own Qt or OpenGL-based projects.
