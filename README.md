# Vulkan Game Engine

## Overview

This is a game engine using the Vulkan API to learn about computer graphics and is currently still in development.

## Implementations

### 1. **Window Creation**: Uses GLFW to create a window.

The Vulkan Game Engine utilizes GLFW for window creation and management. The window is created through the `VgeWindow` class, which encapsulates all necessary functionalities for initializing and managing a GLFW window suitable for Vulkan rendering.

The `VgeWindow` constructor initializes the GLFW library, sets window hints to disable the OpenGL context, and allows resizing and creates the window.

### 2. **Vulkan Initialization**: Initializes Vulkan by creating an instance and checking for extension support.


### 3. **Shader Creation**: Creates vertex and fragment shaders using GLSL.


### 4. **Validation Layers**: Sets up Vulkan validation layers.


### 5. **Physical Device Selection**: Selects a physical device and finds queue families.


### 6. **Logical Device Creation**: Creates a Vulkan logical device.


### 7. **Window Surface**: Creates a window surface for Vulkan rendering.


### 8. **Presentation Queue**: Creates the presentation queue.


### 9. **Swapchain**: Creates the swapchain for image presentation.


### 10. **Imageviews**: Creates imageviews for swapchain images.


### 11. **Graphics Pipeline Configuration**: Configures the fixed function stages (input assembler, rasterization, color blending).


### 12. **Triple Buffering**: Utilizes triple buffering with the swapchain.


### 13. **Renderpass Creation**: Creates a renderpass for managing rendering.


### 14. **Command Buffers**: Implements command buffers for rendering commands.


### 15. **Vertex Buffers**: Implements vertex buffers for vertex data.


### 16. **Fragment Interpolation**: Uses barycentric coordinates for fragment interpolation.


### 17. **Viewport and Swapchain Recreation**: Dynamically adjusts viewport and recreates swapchain on window resize.


### 18. **Push Constants**: Implements push constants for efficient shader data transfer.


### 19. **3D Transformations**: Performs scale, rotation, and transformation using matrices.


### 20. **Projection Matrices**: Creates projection matrices for perspective rendering.


### 21. **Camera View**: Implements a camera view for scene navigation.


### 22. **Game Loop**: Manages camera movement in a frame rate independent manner.


### 23. **Keyboard Input**: Handles keyboard input for movement.


### 24. **Index and Staging Buffers**: Implements index and staging buffers for data management.


### 25. **3D Model Loading**: Loads 3D models from Wavefront .obj files.


### 26. **Diffuse Lighting**: Implements Lambertian diffuse lighting.


### 27. **Lighting**: Supports directional and point light sources.


### 28. **Uniform Buffers**: Implements uniform buffers for larger data storage.


### 29. **Descriptor Sets**: Manages descriptor sets, layouts, and pools.


### 30. **Fragment Lighting Calculation**: Calculates light intensity on a per-fragment basis.


### 31. **Billboard Light Sources**: Supports multiple billboard light sources.


### 32. **Specular Lighting**: Implements Blinn-Phong specular lighting for surface reflections.



## Important Notes

- **Cleanup**: Resource cleanup is the responsibility of the programmer. The project adheres to RAII principles.
- **Ongoing Development**: This is a personal project and will continue to evolve. Future features include player and NPC objects, as well as basic physics simulations like gravity and collision.

### TODO

1. Add more to this readme to benefit understanding of the code base.

## Future Features

- Player objects
- NPC objects
- Basic physics simulations (gravity, collision)

## Getting Started

There are no creation steps as of yet.

## License

This project is licensed under the MIT License

## Acknowledgements

* GLFW
* Vulkan API
* GLSL

Special thanks to learncpp, the Vulkan Tutorial, and Brendan Galea.
