# Levikno
A simple graphics library written in C++ for Vulkan and OpenGL application programing and video game development.

See [docs](docs/GettingStarted.md) to get started after installation

## Building
Before building, make sure that the latest supported vulkan drivers on installed on your system.

If you do not have vulkan installed, you can download and install vulkan through the SDK installer from the [LunarG website](https://vulkan.lunarg.com/). Run the installer to install the vulkan depedencies, if you receive error messages, check to make sure your graphics drivers are updated to the latest versions and that it supports vulkan.

If your system or graphics card does not support vulkan, vulkan will be skipped during building. This means you will not be able to choose vulkan as an option when using the library

### Linux
Instructions may change depending on the linux distribution, first ensure that the required vulkan dependencies are installed on your system. Download the vulkan packages through your distro's package manager:

On Debian based distros:
```
sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers-dev spirv-tools
```

On Red Hat based distros:
```
sudo dnf install vulkan-tools vulkan-loader-devel mesa-vulkan-devel vulkan-validation-layers-devel
```

On Arch based distros:
```
sudo pacman -S vulkan-devel
```

Now copy the github repository:
```
git clone https://github.com/bokidawok/Levikno.git levikno
cd levikno
```

Create a build directory and build using `cmake` and `make`
```
mkdir build && cd build
cmake ..
make
```

### Windows
Copy the github repository:
```
git clone https://github.com/bokidawok/Levikno.git levikno
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

More examples on how to use levikno can be found in the [examples folder](examples/)

Check the simple [tutorial](docs/GettingStarted.md) as well to get started with using the library

## License
Levikno is MIT licensed, levikno is free to modify, copy, and redistribute, more information in [LICENSE.txt](LICENSE.txt)
