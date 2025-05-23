# Levikno
A simple graphics library written in C++ for Vulkan and OpenGL application programing and video game development.

See [docs](docs/GettingStarted.md) to get started after installation

## Building
Before Building, the vulkan and glslang libraries will need to be installed on your system. If your system or graphics card does not support vulkan, it will be skipped during building. Levikno will not allow you to use vulkan when choosing the graphics api.

### Prerequisite
- C++17 compiler
- [cmake](https://cmake.org/)

### Linux
On Linux, make sure that vulkan and glslang are installed. Download the vulkan and glslang packages through your distro's package manager:

On Debian based distros:
```
sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers-dev spirv-tools glslang-tools
```

On Fedora based distros:
```
sudo dnf install vulkan-tools vulkan-loader-devel mesa-vulkan-devel vulkan-validation-layers-devel glslang
```

On Arch based distros:
```
sudo pacman -S vulkan-devel glslang
```

Now copy the github repository:
```
git clone https://github.com/binmian/Levikno.git levikno
cd levikno
```

Create a build directory and build using `cmake` and `make`
```
mkdir build && cd build
cmake ..
make
```

### Windows
On Windows, make sure that vulkan and glslang are installed. Download the VulkanSDK installer from the [LunarG website](https://vulkan.lunarg.com/) and install the vulkan libraries, if you receive error messages, check to make sure your graphics drivers are updated to the latest versions and that your system can support vulkan.

To install glslang, follow the instructions to build glslang on the [github repository](https://github.com/KhronosGroup/glslang?tab=readme-ov-file), you may either build and install glslang from source or through vcpkg.

**It is important that vulkan and glslang are properly installed so that cmake can find and link the packages correctly.**

Now copy the github repository:
```
git clone https://github.com/binmian/Levikno.git levikno
cd levikno
```

Create a build directory and build using `cmake`, make sure that you have compile target assigned to cmake to compile your code such as Visual Studio:
```
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage
The library can be initialized after including the header file and linking the library.

Basic example:
```
#include <levikno/levikno.h>

int main(int argc, char** argv)
{
    LvnContextCreateInfo lvnCreateInfo{};
    lvnCreateInfo.windowapi = Lvn_WindowApi_glfw; // choose window api
    lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan; // choose graphics api

    lvn::createContext(&lvnCreateInfo);

    // some rendering code here

    lvn::terminateContext();

    return 0;
}
```

More examples on levikno can be found in the [examples folder](examples/)

Follow the [tutorial](docs/GettingStarted.md) to get started on using the library

## License
Levikno is MIT licensed, levikno is free to modify, copy, and redistribute, more information in [LICENSE.txt](LICENSE.txt)
