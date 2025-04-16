# VeiM [![License](https://img.shields.io/github/license/SkaLe3/VeiM)](https://github.com/SkaLe3/VeiM/blob/main/LICENSE)

![VeiM](/Resources/Veim_Logo_Purple_TransparentBG.png?raw=true "VeiM")

VeiM is a simple game engine for Windows created as a personal educational project. This project is currently in the early stage of development.
***

## Getting Started
<ins>**1. Downloading the repository:**</ins>

Start by cloning the repository with `git clone --recursive https://github.com/SkaLe3/VeiM`.
If the repository was cloned non-recursively previously, use `git submodule update --init` to clone the necessary submodules.

<ins>**2. Configuring the dependencies:**</ins>

Run the [Setup.bat](https://github.com/SkaLe3/VeiM/blob/master/Scripts/Setup.bat) file found in `Scripts` folder. This will register engine installation and files associations.

Run the [Win-GenerateProjectFiles.bat](https://github.com/SkaLe3/VeiM/blob/master/Scripts/Win-GenerateProjectFiles.bat) file found in `Scripts` folder. This will generate a Visual Studio solution file.

If changes are made, or if you want to regenerate project files, rerun the [Win-GenerateProjectFiles.bat](https://github.com/SkaLe3/VeiM/blob/master/Scripts/Win-GenerateProjectFiles.bat) script file found in `Scripts` folder.

### Requirements

- [Visual Studio 2022](https://visualstudio.com) (required to automatically set up the project with the sctipt, otherwise you will have to set up the project yourself)

### Dependencies

Below are the major dependencies for VeiM:

- **OpenGL**
- **[GLFW](https://github.com/glfw/glfw)**
- **[ImGui](https://github.com/ocornut/imgui)**
- **[stb](https://github.com/nothings/stb)**
- **[glm](https://github.com/g-truc/glm)**
- **[spdlog](https://github.com/gabime/spdlog)**
- **[yaml-cpp](https://github.com/jbeder/yaml-cpp)**
- **[assimp](https://github.com/assimp/assimp)**

These dependencies are essential for various core functionalities of VeiM and are managed automatically using Git submodules (or included into the project). When cloning the repository, ensure to perform a *recursive* clone to include them
***

## The Plan
The plan for the VeiM project is to develop a sufficiently powerful and capable 3D game engine. This project aims to provide hands-on experience in game development, game engine architecture, and software development. Additionally, VeiM serves as my graduate project for my studies at KPI, showcasing my skills and knowledge in these areas.

### Main features to come
- Not available yet

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
- *GPU Gems*, by Randima Fernando (NVIDIA)

**YouTube Channels:**

- [The Cherno](https://www.youtube.com/@TheCherno)
- [ThinMatrix](https://www.youtube.com/@ThinMatrix)

**YouTube Videos:**

- [ThinMatrix - Skeletal animations](https://www.youtube.com/@ThinMatrix)
  
**Web sites**

- [learnopengl.com](https://learnopengl.com/)

**Articles**

- [What every coder should know about gamma](https://blog.johnnovak.net/2016/09/21/what-every-coder-should-know-about-gamma/)

***

## License
VeiM is licensed under the Apache-2.0 License. See the [LICENSE](https://github.com/SkaLe3/VeiM/blob/main/LICENSE) file for more details.
