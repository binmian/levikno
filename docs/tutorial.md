

# Levikno Documentation
The Levikno library is a graphics framework built in C++ for creating windows and rendering objects. This document will give a brief tutorial on how to use the library.


## Design Architecture
### Creating and Destroying Objects
Levikno's API structure is built directly on top of Vulkan which requires a low level and explicit interface, this also requires objects to be created and destroyed when used for rendering (eg. vertex buffers, shaders, framebuffers, pipelines, etc)
In Levikno, objects are usually created through this function format:
```
LvnResult createObject(Object** object, ObjectCreateInfoStruct* createInfo)
```
For each object that needs to be created, in most cases a create function will be associated with it. In the example above, the object will need to be passed in by pointer in the first parameter. 

The second parameter takes in a createInfo struct that tells Levikno what requirements are needed when creating this object. 

Each object will also have a createInfo struct associated with it.

Note that objects need to be declared as a pointer which points to the created object.

Example:
```
 // declare create info struct for LvnWindow
LvnWindowCreateInfo windowInfo{};
windowInfo.width = 800;
windowInfo.height = 600;
windowInfo.title = "window";

// declare LvnWindow object as a pointer
LvnWindow* window;

// create the object by passing the object in and its createInfo
lvn::createWindow(&window, &windowInfo);
```
You may have also noticed that the create object function returns a ```LvnResult``` enum. This enum returns a value which can be determined whether the function was executed successfully or not. For each create object function, it returns either ```Lvn_Result_Success``` or ```Lvn_Result_Failure``` when creating the object. In case if an error happens, the program can respond whenever the function returns on failure.

Example:
```
LvnResult result = lvn::createWindow(&window, &windowInfo);
if (result == Lvn_Result_Failure)
{
    // do something ...
}
```

When an object is no longer needed it must be manually destroyed to free back memory, destroying an object requires passing it through its destroy function:
```
void destroyObject(Object* object)
```

Example:
```
lvn::destroyWindow(window);
```

### Structure
When using the Levikno library, functions, structs, enums, and macros each have their own specific syntax or "style"
* All functions are declared within the namespace ```lvn``` (ex. ```lvn::createContext()```, ```lvn::renderCmdDraw()```)
* structs and enums have the prefix ```Lvn``` in front of its object name (ex. ```LvnFrameBuffer```, ```LvnResult```)
* enum variables start with the prefix ```Lvn``` followed by the name of the enum and then the value separated by underscores (ex. ```Lvn_Result_Success```, ```Lvn_GraphicsApi_vulkan```)
* macros are in all uppercase and usually start with the prefix ```LVN``` (ex. ```LVN_WARN()```)

## Getting Started
### Initialization
Before anything can be used, a levikno context first needs to be created to initialize the library. For this case, we will be using GLFW and Vulkan for the window and rendering backends.
```
#include <levikno/levikno.h>

int main()
{
    LvnContextCreateInfo lvnCreateInfo{};
    lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

	lvn::createContext(&lvnCreateInfo);
}
```
The ```LvnContextCreateInfo``` struct is used for initializing and to choose what window and graphics API to use when creating windows and rendering to them. Pass the createInfo struct into ```lvn::createContext(&lvnCreateInfo)``` to create the levikno context.

Also keep in mind that we are initializing each struct by using **value initialization** (the double curly braces at the end of ```lvnCreateInfo```). Using value initialization fills the entire struct with its default values so we won't  have to fill every parameter ourselves everytime we create a new struct.

### Logging
Before we move on, it is also recommended to enable logging in case we make a mistake, log messages may have some important info that could help us in debugging. Since we are also using Vulkan which is a very specific and low level API, it can be very easy to create errors causing undefined behavior. Luckily Vulkan allows us to receive debug messages through the use of validation layers.

Logging is optional and is mostly only used for debugging purposes, however keep in mind the performance. Vulkan validation layers add a bit of extra overhead which may slow down the program.

To enable logging and Vulkan validation layers, add the new lines to the code:
```
LvnContextCreateInfo lvnCreateInfo{};
lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;
lvnCreateInfo.enableLogging = true;
lvnCreateInfo.enableVulkanValidationLayers = true;
	
lvn::createContext(&lvnCreateInfo);
```
Note that ```enableLogging``` also has to be true in order to show vulkan validation layers, this parameter controls whether message output will be enabled

When the levikno context is created with logging enabled, the context will create two loggers by default, the core logger and the client logger. The core logger is technically reserved for use within the library although it can still be used outside it. The client logger is the general logger the user would use when building applications.

We won't be using logging that much in this tutorial so we will only use it to display info and error messages.

### Initialize Rendering
Before we create a window, we first have to declare a few preset values before rendering to a window. Because we are using Vulkan, we need to know which physical device (GPU) to use for rendering. In most cases a desktop PC will likely have only 1 GPU, however there are some cases such as in laptops where there are two GPUs (an integrated CPU and dedicated GPU). The physical devices and their information can be obtained after the context is created:
```
#include <vector>
#include <cstdint>

...

uint32_t deviceCount = 0;
std::vector<LvnPhysicalDevice*> devices;
lvn::getPhysicalDevices(nullptr, &deviceCount);

devices.resize(deviceCount);
lvn::getPhysicalDevices(devices.data(), &deviceCount);

for (uint32_t i = 0; i < deviceCount; i++)
{
	LvnPhysicalDeviceInfo deviceInfo = lvn::getPhysicalDeviceInfo(devices[i]);
	LVN_TRACE("name: %s, version: %d", deviceInfo.name, deviceInfo.driverVersion);
}

``` 
We first use ```lvn::getPhysicalDevices``` to get the number of physical devices available which is then used to create a temporary storage of ```LvnPhysicalDevices*``` in an ```std::vector```. We then call ```lvn::getPhysicalDevices``` again, this time with the vector of devices passed in to acquire the physical devices. After that we can check each physical device's information such as the name of the device and version by retrieving a ```LvnPhysicalDeviceInfo``` struct through ```lvn::getPhysicalDeviceInfo```.

Note that the driver and api version in the info struct directly corresponds to the information Vulkan provides in ```VkPhysicalDeviceProperties```.


### Creating a Window
Like creating any other object, we first declare the create info struct:
```
LvnWindowCreateInfo windowInfo{};
```
Then fill in all the necessary parameters:
```
...
LvnWindowCreateInfo windowInfo{};
windowInfo.width = 800;
windowInfo.height = 600;
windowInfo.title = "leviknoTutorial";
windowInfo.fullscreen = false;
windowInfo.resizable = true;
windowInfo.minWidth = 300;
windowInfo.minHeight = 200;
windowInfo.maxWidth = -1;
windowInfo.maxHeight = -1;
windowInfo.pIcons = nullptr;
windowInfo.iconCount = 0;
```
Most of these parameters are self-explanatory
* The ```width``` and ```height``` defines the dimensions of out window in pixels.
* The ```title``` displays the name of the window in the title bar
* the ```maxWidth``` and ```maxHeight``` parameters specify the maximum size of the window. By default they are set to -1 which disables it.
* The ```pIcons``` parameter takes in an array of ```LvnWindowIconData``` structs which specifies the different icon images and sizes to use for the window.
* The ```iconCount``` parameter is the number of icon structs within the ```pIcons``` array.

Note: Whenever parameters have a prefix ```p``` in front of the type name, it indicates that an array of data can be taken in with a pointer to the first element in the array. This parameter is usually followed by another unsigned integer parameter that asks for the number of elements in the array with the suffix ```Count```. For example, ```pIcons``` and ```iconCount```

Now declare a ```LvnWindow*``` pointer and pass both the pointer and the create info struct to its respective create function:
```
...

LvnWindowCreateInfo windowInfo{};
windowInfo.width = 800;
windowInfo.height = 600;
windowInfo.title = "leviknoTutorial";
windowInfo.fullscreen = false;
windowInfo.resizable = true;
windowInfo.minWidth = 300;
windowInfo.minHeight = 200;
windowInfo.maxWidth = -1;
windowInfo.maxHeight = -1;
windowInfo.pIcons = nullptr;
windowInfo.iconCount = 0;

LvnWindow* window;
lvn::createWindow(&window, &windowInfo);
```

If we want to also check if an object has been successfully created, we can check the ```LvnResult``` enum the function returns:
```
...

LvnWindow* window;
LvnResult result = lvn::createWindow(&window, &windowInfo);
if (result == Lvn_Result_Failure)
{
	// do something for error
	return -1;
}
```


After successfully creating a window, we can then use our window in the main rendering loop:
```
...

while (lvn::windowOpen(window))
{
	lvn::updateWindow(window);

}

lvn::destroyWindow(window);
```
In the example above, ```lvn::windowOpen(window)``` is a function that returns a boolean value if the window is open or not, the function returns false if the window has been closed.

In the loop, ```lvn::updateWindow(window)``` updates window and poll events related to the api.

Finally make sure to destroy the window using ```lvn::destroyWindow(window)``` at the end of the program after the window closes.

