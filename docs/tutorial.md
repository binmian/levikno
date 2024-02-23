
# Levikno Documentation
The Levikno library is a graphics framework built in C++ for creating windows and rendering objects. This document will give a brief tutorial on how to use the library.


## Design Architecture
### Creating and Destroying Objects
Levikno's API structure is built on top of other graphics APIs such as Vulkan and OpenGL which requires objects to be created and destroyed that are used for rendering (eg. vertex buffers, shaders, framebuffers, and pipelines)
In Levikno, objects are usually created through this function format:
```
LvnResult createObject(Object** object, ObjectCreateInfoStruct* createInfo)
```
For each object that needs to be created, in most cases a create function will be associated with it. In the example above, the object will need to be passed in by pointer in the first parameter. 

The second parameter takes in a createInfo struct that tells Levikno what requirements are needed when creating this object. 

Each object will also have a createInfo struct associated with it.

Also note that objects need to be declared as a pointer which points of the created object.

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
You may have also noticed that the create object function returns a ```LvnResult``` enum. This enum returns the success state or other state of a function. For each create object function, it returns whether it was successful or not when creating the object. This is useful for program control flow whenever an operation fails.

Example:
```
LvnResult result = lvn::createWindow(&window, &windowInfo);
if (result == Lvn_Result_Failure)
{
    // do something ...
}
```

When an object is no longer needed it must be manually destroyed, destroying an object requires passing it through its destroy function:
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
In order to use the Levikno library, a levikno context needs to be created first to initialize the library.
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
The ```LvnContextCreateInfo``` struct defines certain requirements for initializing and what window and graphics API to use when creating windows and rendering to them. Pass the createInfo struct into ```lvn::createContext(&lvnCreateInfo)``` to create the levikno context.

We will be using Vulkan for the graphics API and GLFW for the window API since these are what Levikno was originally built upon.

Also keep in mind that we are initializing each struct by using **value initialization** (the double curly braces at the end of ```lvnCreateInfo```). We use value initialization so that we don't have to fill every parameter ourselves.

### Logging
Before we move on, it is also recommended to enable logging in case we make a mistake, log messages may have some important info that could help us. Since we are also using Vulkan which is a very specific and low level API, it can be very easy to create errors or undefined behavior. Luckily Vulkan allows us to receive debug messages through the use of validation layers.

Logging is optional and is mostly only used for debugging purposes, however keep in mind the performance. Vulkan validation layers add a bit of extra overhead so do not forget to disable it in release.

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

### Creating a Window
Like creating any other object, we first declare the create info struct:
```
LvnWindowCreateInfo windowInfo{};
```
Then fill in the necessary parameters, for this I will fill in all the parameters:
```
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

