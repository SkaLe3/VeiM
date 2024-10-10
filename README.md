# VeiM [![License](https://img.shields.io/github/license/SkaLe3/VeiM)](https://github.com/SkaLe3/VeiM/blob/main/LICENSE)

![VeiM](/Resources/Veim_Logo_Purple_TransparentBG.png?raw=true "VeiM")

VeiM is a simple game engine for Windows created as a personal educational project. This project is currently in the early stage of development.
***

## Getting Started
<ins>**1. Downloading the repository:**</ins>

Start by cloning the repository with `git clone --recursive https://github.com/SkaLe3/VeiM`.
If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

<ins>**2. Configuring the dependencies:**</ins>

Run the [Win-GenProjects.bat](https://github.com/SkaLe3/VeiM/blob/master/scripts/Win-GenProjects.bat) file found in `scripts` folder. This will generate a Visual Studio solution file.

If changes are made, or if you want to regenerate project files, rerun the [Win-GenProjects.bat](https://github.com/SkaLe3/VeiM/blob/master/scripts/Win-GenProjects.bat) script file found in `scripts` folder.

### Requirements
- [Visual Studio 2022](https://visualstudio.com) (required to automatically set up the project, otherwise you will have to set up the project yourself)

### Dependencies

Below are the major dependencies for VeiM:

- **OpenGL**: For rendering graphics in both 2D and 3D.
- **[GLFW](https://github.com/glfw/glfw)**: A multi-platform library for creating windows with OpenGL contexts and managing input.
- **Currently unknown**: A physics engine for collision detection and dynamics.
- **Currently unknown**: For audio playback and management.
- **[ImGui](https://github.com/ocornut/imgui)**: A graphical user interface library for implementing in-engine tools and editors.
- The list will be expanded

These dependencies are essential for various core functionalities of VeiM and are managed automatically using Git submodules. When cloning the repository, ensure to perform a *recursive* clone to include them
***

## The Plan
The plan for the VeiM project is to develop a sufficiently powerful and capable 3D game engine. This project aims to provide hands-on experience in game development, game engine architecture, and software development. Additionally, VeiM serves as my graduate project for my studies at KPI, showcasing my skills and knowledge in these areas.

### Main features to come
- Basic 2D and 3D rendering capabilities
  - OpenGL rendering API support 
- Simple physics engine for 2D and 3D objects
- Sound playback and management
- Keyboard and mouse input management
- A simple in-engine editor for placing and modifying game objects
- Basic scripting support for game logic (optional)

### Goals
- Gain a foundational understanding of game engine components, with a particular focus on low-level rendering techniques
- Create a simple, modular, and extendable codebase that can be easily understood and expanded upon
- Push my boundaries by tackling an ambitious project that requires problem-solving and learning
- Develop several games using the VeiM game engine, showcasing its capabilities and my progress

***

## Resources
Here are some of the key resources I am using for this project:

**Books:**

- *Game Engine Architecture, Third Edition*, by Jason Gregory
- *Real-Time Rendering, Fourth Edition*, by Tomas Akenine-Möller, Eric Haines, Naty Hoffman, Angelo Pesce, Michał Iwanicki, and Sébastien Hillaire

**YouTube Channels:**

- (Links to specific YouTube tutorials or channels will be added here)
- [The Cherno](https://www.youtube.com/@TheCherno)
- [ThinMatrix - Skeletal animations](https://www.youtube.com/@ThinMatrix)
  
**Web sites:**

- [learnopengl.com](https://learnopengl.com/)
  
I will update this section with additional resources that help develop VeiM.

***

## License
VeiM is licensed under the Apache-2.0 License. See the [LICENSE](https://github.com/SkaLe3/VeiM/blob/main/LICENSE) file for more details.
