# Levikno Guide
The Levikno library is a graphics framework built in C++ for creating windows and rendering objects. This document is a tutorial on how to use and integrate Levikno to your project. Before beginning this guide, it is advised that you have prior experience in understanding C++ and how graphics APIs work such as [Vulkan](https://vulkan-tutorial.com/Introduction) and [OpenGL](https://learnopengl.com/Introduction). I will not be going too in depth with how graphics APIs work so it is expected that you know the general terminology for the graphics APIs presented in this tutorial.

## Index
- [Api Structure](#api-structure)
	- [Creating/Destroying Objects](#creating-and-destroying-objects)
	- [Structure](#structure)
- [Getting Started](#getting-started)
	- [Initialization](#initialization)
	- [Logging](#logging)
	- [Choosing Physical Device](#choosing-physical-device)
	- [Creating Window](#creating-window)
	- [Creating Buffers](#creating-buffers)
	- [Shaders and Pipelines](#shaders-and-pipelines)
	- [Drawing](#drawing)


## Api Structure
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
The ```LvnContextCreateInfo``` struct is used for initializing and to choose what window and graphics API to use when creating windows and rendering to them. Pass the createInfo st### Loggingruct into ```lvn::createContext(&lvnCreateInfo)``` to create the levikno context.

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
Choosing Physical Device
### Choosing Physical Device
Before we create a window, we first have to declare a few preset values before rendering to a window. Because we are using Vulkan, we need to know which physical device (GPU) is going to be used to render our scene and presented to the screen. In most cases a desktop PC will likely have only 1 GPU, however there are some cases such as in laptops where there are two GPUs (an integrated CPU and dedicated GPU). The physical devices and their information can be obtained after the context is created:
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

After a suitable physical device has been found, attach it to the ```LvnRenderInitInfo``` struct and call the render init function to initialize rendering:
```
	LvnRenderInitInfo renderInfo{};

	// find and check if physical device is supported
	for (uint32_t i = 0; i < deviceCount; i++)
	{
		if (lvn::checkPhysicalDeviceSupport(devices[i]) == Lvn_Result_Success)
		{
			renderInfo.physicalDevice = devices[i];
			break;
		}
	}

	lvn::renderInit(&renderInfo);
```
The function ```checkPhysicalDeviceSupport``` checks if a physical device is supported, if the device has the required extensions the function will return on success. In the example above, we iterate through the list of physical devices until a physical device is found that is supported. Note that if a physical device is not suitable or a requirement is missing from the physical device such as an extension or feature, the function will return on failure. 

### Creating Window
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
	lvn::windowUpdate(window);

}

lvn::destroyWindow(window);
```
In the example above, ```lvn::windowOpen(window)``` is a function that returns a boolean value if the window is open or not, the function returns false if the window has been closed.

In the loop, ```lvn::windowUpdate(window)``` updates window and poll events related to the api.

Finally make sure to destroy the window using ```lvn::destroyWindow(window)``` at the end of the program after the window closes.

### Creating Buffers
Buffers are used to store vertex and index data to draw to the screen. In Levikno, the vertex and index buffer are both stored in one ```LvnBuffer``` object. Create a ```LvnBufferCreateInfo``` struct with the following parameters:
```
LvnBufferCreateInfo bufferCreateInfo{};
bufferCreateInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
```
```type``` indicates what this buffer should be used for, since we want to have a buffer that contains both the vertex and index data, we set the type to ```Lvn_BufferType_Vertex | Lvn_BufferType_Index```.

Now let's create the vertex and index data for our buffer to store, I will keep this to a simple triangle:
```
float vertices[] = 
{
/*      Pos (x,y,z)   |   color (r,g,b)   */
	 0.0f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // v1
	 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // v2
	-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // v3
};

uint32_t indices[] = 
{
	0, 1, 2
};
```
Notice in out vertices data, each vertex contains six floats, the first three floats define the vertex position and the next three floats contain the color data. We need to tell our buffer how the vertex data is laid out by passing in the vertex attributes and binding descriptions. Let's first create the vertex attributes, the ```LvnVertexAttribute``` struct looks like this:
```
struct LvnVertexAttribute
{
	uint32_t binding;
	uint32_t layout;
	LvnVertexDataType type;
	uint32_t offset;
};
```
- ```binding``` is the binding number which this attribute takes its data from
- ```layout``` is the shader input location number for this attribute
- ```type``` is defines the size and type of the vertex attribute data
- ```offset``` is the byte offset of this attribute relative to the start of the vertex input binding

Now add the following lines to the code:
```
LvnVertexAttribute attributes[2] = 
{
	{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },
	{ 0, 1, Lvn_VertexDataType_Vec3f, (3 * sizeof(float)) },
};

...

bufferCreateInfo.pVertexAttributes = attributes;
bufferCreateInfo.vertexAttributeCount = 2;
```
Note how we have two vertex attributes, one for the position and the second for the color data, both have the size and type ```vec3``` in the shader.

Next we also need to define the vertex binding descriptions, the struct for ```LvnVertexBindingDescription``` looks like this:
```
struct LvnVertexBindingDescription
{
	uint32_t binding;
	uint32_t stride;
};
```
- ```binding``` is the binding number that this structure describes to a place in memory
- ```stride``` is the consecutive length in bytes of the entire vertex (size in bytes of one vertex)

Add the new lines in the code below:
```
...

LvnVertexBindingDescription vertexBindingDescription{};
vertexBindingDescription.binding = 0;
vertexBindingDescription.stride = 6 * sizeof(float);

...

bufferCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
bufferCreateInfo.vertexBindingDescriptionCount = 1;
```
Our vertex has a stride of 6 floats (24 bytes), 3 floats for the position and 3 floats for the color data.

The last parameters require the vertex and index data themselves as well as the total size (in bytes) of the vertex and index data:
```
...

bufferCreateInfo.pVertices = vertices;
bufferCreateInfo.vertexBufferSize = sizeof(vertices);

bufferCreateInfo.pIndices = indices;
bufferCreateInfo.indexBufferSize = sizeof(indices);
```

Now with all the necessary parameters filled, we can now create our buffer, the final code should look something like this:
```
// vertex data
float vertices[] = 
{
/*      Pos (x,y,z)   |   color (r,g,b)   */
	 0.0f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // v1
	 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // v2
	-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // v3
};

// index data
uint32_t indices[] = 
{
	0, 1, 2
};

// vertex attribute data
LvnVertexAttribute attributes[2] = 
{
	{ 0, 0, Lvn_VertexDataType_Vec3f, 0 },
	{ 0, 1, Lvn_VertexDataType_Vec3f, (3 * sizeof(float)) },
};

// define the vertex binding description
LvnVertexBindingDescription vertexBindingDescroption{};
vertexBindingDescroption.binding = 0;
vertexBindingDescroption.stride = 6 * sizeof(float);

// combine all the data in the createInfo struct
LvnBufferCreateInfo bufferCreateInfo{};
bufferCreateInfo.type = Lvn_BufferType_Vertex | Lvn_BufferType_Index;
bufferCreateInfo.pVertexBindingDescriptions = &vertexBindingDescroption;
bufferCreateInfo.vertexBindingDescriptionCount = 1;
bufferCreateInfo.pVertexAttributes = attributes;
bufferCreateInfo.vertexAttributeCount = 2;
bufferCreateInfo.pVertices = vertices;
bufferCreateInfo.vertexBufferSize = sizeof(vertices);
bufferCreateInfo.pIndices = indices;
bufferCreateInfo.indexBufferSize = sizeof(indices);

LvnBuffer* buffer;
lvn::createBuffer(&buffer, &bufferCreateInfo);
```

### Shaders and Pipelines
The next step before we can draw anything onto our window is to create a rendering pipeline. A rendering or graphics pipeline outlines the necessary procedures for translating vertices and color onto the screen.

In order to create the pipeline, let's first see what the ```LvnPipelineCreateInfo``` struct contains:
```
struct LvnPipelineCreateInfo
{
	LvnPipelineSpecification* pipelineSpecification;
	LvnVertexBindingDescription* pVertexBindingDescriptions;
	uint32_t vertexBindingDescriptionCount;
	LvnVertexAttribute* pVertexAttributes;
	uint32_t vertexAttributeCount;
	LvnDescriptorLayout** pDescriptorLayouts;
	uint32_t descriptorLayoutCount;
	LvnShader* shader;
	LvnRenderPass* renderPass;
};
```
There is a lot to unpack here so let's go over each parameter one at a time.

#### Pipeline Specification (fixed functions)
```pipelineSpecification``` is a pointer to a ```LvnPipelineSpecification``` struct which holds pipeline's state or "fixed functions" such as the cull mode, depth stencil operations, viewports, multisampling, draw topology, and rasterization.

In most cases we don't need to change any of these parameters after the pipeline has been creates hence the name "fixed" functions. Levikno provides a few default parameter functions which returns a fully initialized create info struct with all of the default parameters set.

Create a ```LvnPipelineSpecification``` struct and a ```LvnPipelineCreateInfo``` struct:
```
LvnPipelineSpecification pipelineSpec = lvn::pipelineSpecificationGetConfig();

LvnPipelineCreateInfo pipelineCreateInfo{};
pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
```
```lvn::pipelineSpecificationGetConfig()``` returns a initialized ```LvnPipelineSpecification``` struct with all of its default parameters set for rendering.

#### Vertex Attributes and Binding Description
Next are the ```pVertexBindingDescriptions``` and ```pVertexAttributes```, recall from the [Creating Buffers](#creating-buffers) section where the vertex attributes and binding descriptions are needed to create the buffer. The same parameters are needed for creating the pipeline in order for the pipeline to know how the vertex data is laid out when it is used for rendering. If you have defined the vertex attributes and binding descriptions from the previous section, simply assign them again to the pipeline create info struct:
```
...

pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
pipelineCreateInfo.vertexBindingDescriptionCount = 1;
pipelineCreateInfo.pVertexAttributes = attributes;
pipelineCreateInfo.vertexAttributeCount = 2;
```

#### Descriptor Layouts
We will be ignoring the ```pDescriptorLayouts``` and ```descriptorLayoutCount``` parameters for now since we aren't using them. Descriptor layouts and descriptor sets can be basically thought of as large chunks of resources that can be binded to the shader such as uniform buffer data and textures.

#### Shader
The ```shader``` parameter is where we add our ```LvnShader``` object, in most cases, the pipeline will use two shaders, the vertex and fragment shader used for transforming vertices and filling in the color between those vertices.

We first have to create the vertex and fragment shader sources

Vertex shader:
```
#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main()
{
	gl_Position = vec4(inPos, 1.0);
	fragColor = inColor;
}
```

Fragment shader:
```
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;

void main()
{
	outColor = vec4(fragColor, 1.0);
}
```

There are several ways we can load our shaders into Levikno through these functions:
- ```createShaderFromSrc()``` creates the shader given the direct sources of the shader modules
- ```createShaderFromFileSrc()``` creates the shader given the filepaths to the external files that contain the shader source
- ```createShaderFromFileBin()``` creates a shader given the filepaths to the external binary files that contain the shader source (for vulkan this would be a SPIRV binary file ending in .spv)

For this tutorial, we can just leave the shader sources within the main file as a string and load the shaders from src.
```
const char* vertexShader = R"(
#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main()
{
	gl_Position = vec4(inPos, 1.0);
	fragColor = inColor;
})";

const char* fragmentShader = R"(
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;

void main()
{
	outColor = vec4(fragColor, 1.0);
})";

...

LvnShaderCreateInfo shaderCreateInfo{};
shaderCreateInfo.vertexSrc = vertexShader;
shaderCreateInfo.fragmentSrc = fragmentShader;

LvnShader* shader;
lvn::createShaderFromSrc(&shader, &shaderCreateInfo);
```
Note that ```LvnShaderCreateInfo``` is taken in by all three create shader functions and contains only two string parameters for the vertex and fragment shader. If you were loading the shaders through an external file instead, you would just replace the ```vertexSrc``` and ```fragmentSrc``` parameters with the filepaths to the external files, for example:
```
shaderCreateInfo.vertexSrc = "/home/user/Documents/shaders/vertexShader.vert";
shaderCreateInfo.fragmentSrc = "/home/user/Documents/shaders/fragmentShader.frag";

LvnShader* shader;
lvn::createShaderFromFileSrc(&shader, &shaderCreateInfo); // Note that we are loading from file src
```

We can now add the shader to the pipeline create info struct:
```
...

pipelineCreateInfo.shader = shader;
```

#### Render Pass
The last parameter is the ```renderPass``` parameter which takes in a ```LvnRenderPass``` object. A render pass is used to handle different color and depth attachments used for drawing our scene. Levikno does most of the background work for us when using render passes so we only care about where our render pass will be used to render, such as to a window or to an off screen framebuffer. Fortunately, when we creates a window from the [Creating Window](#creating-window) section, Levikno automatically creates a render pass along with it. The render pass can now be retrieved by calling this function:
```
LvnRenderPass* renderPass = lvn::windowGetRenderPass(window);
pipelineCreateInfo.renderPass = renderPass;
```

With all the parameters set, we can now combine all the parameters and create the pipeline:
```
...

LvnPipelineCreateInfo pipelineCreateInfo{};
pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
pipelineCreateInfo.vertexBindingDescriptionCount = 1;
pipelineCreateInfo.pVertexAttributes = attributes;
pipelineCreateInfo.vertexAttributeCount = 2;
pipelineCreateInfo.pDescriptorLayouts = nullptr;
pipelineCreateInfo.descriptorLayoutCount = 0;
pipelineCreateInfo.shader = shader;
pipelineCreateInfo.renderPass = renderPass;

LvnPipeline* pipeline;
lvn::createPipeline(&pipeline, &pipelineCreateInfo);

lvn::destroyShader(shader);
```
After creating the pipeline, the shader is no longer needed (unless you plan to use it again later) so we can destroy the shader right after creating the pipeline.

### Drawing
With all of our resources created, we can now draw to the window using our pipeline and buffer. In the main while loop, add the following lines of code:
```
...

while (lvn::windowOpen(window))
{
	lvn::windowUpdate(window);

	lvn::renderBeginNextFrame(window);
	lvn::renderBeginCommandRecording(window);

	lvn::renderClearColor(window, 0.0f, 0.0f, 0.0f, 1.0f);
	lvn::renderCmdBeginRenderPass(window);

	lvn::renderCmdBindPipeline(window, pipeline);

	lvn::renderCmdBindVertexBuffer(window, buffer);
	lvn::renderCmdBindIndexBuffer(window, buffer);

	lvn::renderCmdDrawIndexed(window, sizeof(indices) / sizeof(indices[0])); // number of elements in indices array (3)

	lvn::renderCmdEndRenderPass(window);
	lvn::renderEndCommandRecording(window);
	lvn::renderDrawSubmit(window);
}
```
Note that most render functions have the prefix ```render``` in front of the function to indicate that it is used for rendering, you will mostly use these functions only within the rendering loop.

Also note that most rendering functions will take in the window as the first parameter since the window contains most of the necessary resources for rendering such as the swapchain images, framebuffer, render passes, and surface data.

Let's break this code down:
- ```renderBeginNextFrame()``` begins the next window frame (in Vulkan, it acquires the next image from the swap chain)
- ```renderBeginCommandRecording()``` begins recording render commands under the window passed in. Note that rendering functions with the prefix ```renderCmd``` must be called only during command buffer recording
- ```renderClearColor()``` takes in four floats which are the red, green, blue, and alpha, components for the background color
- ```renderCmdBeginRenderPass()``` begins the render pass for the window given
- ```renderCmdBindPipeline()``` binds the pipeline
- ```renderCmdBindVertexBuffer()``` and ```renderCmdBindIndexBuffer()``` binds the vertex and index buffer respectively. Note that the buffer we created contains both the vertex and index data, if a buffer is created with only vertex data, only the bind vertex function is needed.
- ```renderCmdDrawIndexed()``` will "draw" the vertices in our binded buffer using the index data provided and given the index count of our indices. Because our triangle only has three indices to draw three vertices, we pass 3 into our function
- ```renderCmdEndRenderPass()``` and ```renderEndCommandRecording()``` ends the render pass and command buffer respectively
- ```renderDrawSubmit()``` is the function that actually submits all out render commands recorded in our command buffers and sends them to the GPU (physical device). Once the GPU finishes rendering the frame, it is then presented to the window

Don't forget to also destroy all of the resources we created when the window is closed and the program ends. We also need to terminate the levikno context before the program exits to ensure if all the resources have been destroyed:
```
...

/* we destroyed our shader earlier after creating the pipeline */

lvn::destroyBuffer(buffer);
lvn::destroyPipeline(pipeline);
lvn::destroyWindow(window);

lvn::terminateContext();

return 0;
```

Now build and run the program, you should now see your triangle appear.

![simpleTriangleExample](https://github.com/bokidawok/Levikno/blob/main/.github/simpleTriangleExample.png)

The [source](https://github.com/bokidawok/Levikno/blob/main/examples/simpleTriangle.cpp) code for this section can be found in the examples folder
