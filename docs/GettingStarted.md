# Levikno Guild

## Index
- [Introduction](#1-introduction)
- [Downloading](#2-downloading)
- [Building](#3-building)
- [Getting Started](#4-getting-started)
    - [Api Structure](#41-api-structure)
    - [Creating the Context](#42-creating-the-context)
    - [Opening a Window](#43-opening-a-window)
    - [Creating Buffers](#44-creating-buffers)
    - [Shaders and Pipelines](#45-shaders-and-pipelines)
    - [Drawing and Main Render Loop](#46-drawing-and-main-render-loop)

# 1. Introduction

Levikno is a graphics framework built in C++ for game development and multimedia applications. The Levikno library is built on top of OpenGL and Vulkan and is designed to support and wrap around multiple graphics APIs.

The core API structure of levikno is designed similar to Vulkan API calls, meaning concepts such as pipelines, buffers, descriptor layouts/sets and command buffers will be used. This is made so that less explicit APIs such as OpenGL can be integrated without abstracting too much from lower level APIs such as Vulkan.

# 2. Downloading

The Levikno source code can be downloaded from the [github](https://github.com/binmian/levikno/tree/main) repository.

# 3. Building

### Prerequisites:

*   cmake
*   C++17 compiler

First clone the repository to a directory and enter into it

```
git clone https://github.com/binmian/levikno.git levikno
cd levikno
```

## 3.1 Installing Vulkan and glslang

Before Building, levikno optionally supports Vulkan and the glslang library for compiling glsl shader code to SPIR-V binaries. If Vulkan is not supported on your system or graphics card, Vulkan will be skipped during the build process. Due note that you won't be able to use Vulkan when choosing the graphics API. Alternately, if Vulkan is supported but the the glslang libraries are not supported or are not found, Vulkan will still be included during the build process, however loading shader code from source will not be available.

If you don't need to use Vulkan or glslang, you can skip this part and move on to the next section [3.2 Building with CMake](#BuildingWithCMake).

### Installing on Windows

To install Vulkan on Windows, download the VulkanSDK installer from the [LunarG website](https://vulkan.lunarg.com/) and install the Vulkan libraries, if you receive error messages, check to make sure your graphics drivers are up to date and that your system supports Vulkan.

For glslang, the glslang libraries need to be properly installed so that cmake can find the glslang package on Windows. Follow the instructions to build glslang from the [github repository](https://github.com/KhronosGroup/glslang?tab=readme-ov-file), you can either build and install glslang from source or through vcpkg.

### Installing on Linux

On Linux, Vulkan and glslang can usually be installed through your distro's package manager.

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

## 3.2 Building with CMake

Inside the git directory, create a new build directory and run cmake

```
mkdir build && cd build
cmake ..
```

On Windows, build using `cmake --build .`

On Linux, build using `make`

# 4. Getting Started

After levikno has been built, link the levikno library to your project. Also add the include directory for levikno from the main git repository.

To use levikno, include "levikno/levikno.h":

```
#include "levikno/levikno.h"
```

## 4.1 API Structure

Before starting, it may be important to understand how levikno's API is structured

### Creating and Destroying Objects

Levikno's API structure is mostly built on top of Vulkan's API which requires a low level and explicit interface, this also requires objects to be created and destroyed when used for rendering (eg. vertex buffers, shaders, framebuffers, pipelines, etc) In levikno, objects are usually created through this function format:

```
LvnResult createObject(Object** object, ObjectCreateInfoStruct* createInfo)
```

For any object that needs to be created, in most cases a create function will be associated with it. In the example above, the object will need to be passed in by pointer in the first parameter.

The second parameter usually takes in a createInfo struct that tells levikno how to create this object. Each object will also have a createInfo struct associated with it.

Note that objects also need to be declared as a pointer type (eg. LvnBuffer\* buffer). In levikno, most objects from the core library are opaue structs.

Example:

```
// declare createInfo struct for LvnBuffer and set parameters
LvnBufferCreateInfo bufferInfo{};
bufferInfo.type = Lvn_BufferType_Vertex;
bufferInfo.usage = Lvn_BufferUsage_Static;
bufferInfo.data = vertices;
bufferInfo.size = sizeof(vertices);

// declare LvnBuffer object as a pointer type
LvnBuffer* buffer;

// create the object by passing it in with its createInfo struct
lvn::createBuffer(&buffer, &bufferInfo);
```

When creating an object, the function also returns a `LvnResult` enum which can be used to determine whether the function succeeded or failed. Each create object function usually returns `Lvn_Result_Success` or `Lvn_Result_Failure`. You can use the return type to handle errors.

Example:

```
LvnResult result = lvn::createBuffer(&buffer, &bufferInfo);
if (result == Lvn_Result_Failure)
{
    // handle error ...
}
```

When an object is no longer needed it must be manually destroyed, destroying an object requires passing it into its associated destroy function:

```
void destroyObject(Object* object)
```

Example:

```
lvn::destroyBuffer(buffer);
```

### Syntax Structure

When using the levikno library, functions, structs, enums, and macros each have their own specific syntax to differentiate them from one another:

*   all functions are declared within the `lvn` namespace (ex. `lvn::createContext()`, `lvn::renderCmdDraw()`)
*   structs and enums have the prefix `Lvn` in front of its object name (ex. `LvnBuffer`, `LvnResult`)
*   enum variables start with the prefix `Lvn` followed by the name of the enum and then the value separated by underscores (ex. `Lvn_Result_Success`, `Lvn_GraphicsApi_vulkan`)
*   macros are in all uppercase and usually start with the prefix `LVN` separated by underscores (ex. `LVN_TRACE()`)

## 4.2 Creating the Context

To use levikno, a levikno context must first be created to initialize the library. First create a `LvnContextCreateInfo` struct and set the required arguments. Levikno requires a graphics and windows api to be set before creating the context, in this case we will use Vulkan and glfw.

```
#include "levikno/levikno.h"

int main(int argc, char** argv)
{
    LvnContextCreateInfo lvnCreateInfo{};
    lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
    lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

    lvn::createContext(&lvnCreateInfo);
}
```

Pass the create context struct into `lvn::createContext()` to initialize the library.

Also keep in mind that each struct needs to be intialized before being passed in to a function, this can be done with value initialization (the double curly braces at the end of lvnCreateInfo). Using value initialization fills the entire struct with its default values so we dont't have to fill out the unnecassary parameters.

### Logging

Before moveing on, it's recommended to enable logging for event messages or potential errors and may contain info that could help with debugging. To enable logging, set the `enableLogging` parameter to true within the create context struct.

Because we are using Vulkan, Vulkan has a special component called validation layers which can help catch errors and misuse of the Vulkan API. To enable error logging of Vulkan validation layers or other graphics api specific errors, set the `enableGraphicsApiDebugLogs` parameter to true as well. Note that if the `enableLogging` parameter is set to false, vulkan validation layers and graphics api specific error logs will not be enabled.

To enable logging, see the added lines to the code:

```
LvnContextCreateInfo lvnCreateInfo{};
lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;
lvnCreateInfo.logging.enableLogging = true;                 // enable logging
lvnCreateInfo.logging.enableGraphicsApiDebugLogs = true;    // enable validation layers and graphics api specific logs

lvn::createContext(&lvnCreateInfo);
```

Logging is optional and is mostly used for debugging purposes. If logging is enabled, keep in mind the performance, Vulkan validation layers can add some extra overhead which may slow performance.

When creating the context with logging enabled, the context will create two loggers by default, the core logger and the client logger. The core logger is mostly reserved for use within the library although it can still be accessed outside of it. The client logger would typically be used in the application side of a project.

If you don't want levikno to output logs from the core library, you can disable the core logger during context creation by setting the `disableCoreLogging` parameter to true:

```
lvnCreateInfo.logging.disableCoreLogging = true;

...
```

### Terminating the Context

If levikno no longer needs to be used, the context needs to be terminated before exiting, this is usually placed at the end of the program:

```
...

lvn::terminateContext();
```

Note that all other objects created from from levikno must be destroyed before calling to terminate the context, otherwise levikno will give log errors of undestroyed objects.

## 4.3 Opening a Window

Like creating any other object, first declare the createInfo struct for a window then fill in the necessary parameters. For creating a window, you must pass in the width, height, and title of the window. We can use levikno's `configWindowInit()` function to set all the parameters correctly:

```
LvnWindowCreateInfo windowInfo = lvn::configWindowInit(800, 600 "myWindow");
```

If you are wondering what the default parameters should be for the window createInfo struct, here is the full declaration below:

```
LvnWindowCreateInfo windowInfo{};
windowInfo.width = 0;
windowInfo.height = 0;
windowInfo.title = "";
windowInfo.minWidth = 0;
windowInfo.minHeight = 0;
windowInfo.maxWidth = -1;
windowInfo.maxHeight = -1;
windowInfo.fullscreen = false;
windowInfo.resizable = true;
windowInfo.vSync = false;
windowInfo.pIcons = nullptr;
windowInfo.iconCount = 0;
windowInfo.eventCallBack = nullptr;
windowInfo.userData = nullptr;
```

Most of these parameters are self-explanatory

*   the `width` and height defines the dimensions of out window in pixels.
*   the `title` displays the name of the window in the title bar
*   the `maxWidth` and `maxHeight` parameters specify the maximum size of the window. By default they are set to -1 which disables it.
*   the `pIcons` parameter takes in an array of `LvnWindowIconData` structs which specifies the different icon images and sizes to use for the window.
*   the `iconCount` parameter is the number of icon structs within the pIcons array.
*   the `eventCallBack` and `userData` parameters take in a callback function for handling events from the window such as key inputs, mouse and scroll inputs, window size, and mouse positions.

Note: Parameters with the prefix `p` in front of the parameter name indicates that an array of data can be taken in with a pointer to the first element in the array. This parameter is usually followed by another unsigned integer parameter that takes in the number of elements in the array with the suffix `Count`. For example, `pIcons` and `iconCount`

Now to create the window, declare a `LvnWindow*` pointer and pass the object and its createInfo struct to its respective create function:

```
...

LvnWindowCreateInfo windowInfo = lvn::configWindowInit(800, 600 "myWindow");

LvnWindow* window;
lvn::createWindow(&window, &windowInfo);
```

After creating our window, we can now create a main rendering loop for our window:

```
...

while (lvn::windowOpen(window))
{
    lvn::windowUpdate(window);
    lvn::windowPollEvents();
}

lvn::destroyWindow(window);

lvn::terminateContext();
```

In the example above, `lvn::windowOpen(window)` is a function that returns a boolean value if the window is open or not, the function returns false if the window has been closed.

In the loop, `lvn::windowUpdate(window)` updates the window every frame and `lvn::windowPollEvents()` checks for window events such as window resizes, focus events, move events, and other inputs.

Finally make sure to destroy the window using `lvn::destroyWindow(window)` at the end of the program after the window closes.

## 4.4 Creating Buffers

Buffers are used to store arbitrary data for the GPU to access and use such as vertex and index data for rendering. For this case, we will create a vertex and index buffer to store vertices for a simple triangle.

### Vertex Buffer

To create a buffer, first declare a `LvnBufferCreateInfo` struct to specify the parameters of the buffer:

```
...

float vertices[] = 
{
  /*   pos (x,y,z)   |  color (r,g,b)   */
    0.0f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // v1
    0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // v2
   -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // v3
};

LvnBufferCreateInfo bufferCreateInfo{};
bufferCreateInfo.type = Lvn_BufferType_Vertex;
bufferCreateInfo.usage = Lvn_BufferUsage_Static;
bufferCreateInfo.size = sizeof(vertices);
bufferCreateInfo.data = vertices;

LvnBuffer* vertexBuffer;
lvn::createBuffer(&vertexBuffer, &bufferCreateInfo);
```

There are a few things to note here in `bufferCreateInfo`:

*   `type` specifies the type of buffer being used, buffers can be multiple types. Since we only store vertex data in this buffer, we only need to provide `Lvn_BufferType_Vertex`.
*   `usage` specifies how the data in the buffer should be used over time, there are three main usages:
    
    *   `Lvn_BufferUsage_Static`: specifies that data in the buffer never changes
    *   `Lvn_BufferUsage_Dynamic`: specifies that data in the buffer can change over time and can be updated with new data
    *   `Lvn_BufferUsage_Resize`: specifies that data in the buffer can be resized over time to a new size and can be updated with new data
    
    For our case, we don't need to update our buffer data later on so we use Lvn\_BufferUsage\_Static.
    
*   `size` specifies the size of the buffer, here `sizeof(vertices)` gets the entire size of our vertices array.
*   `data` specifies the data for the buffer, we give our vertices array which is also a pointer to the beginning of the array. Note that `data` can be left nullptr if you want to update the buffer later, although you would need to set the buffer usage to dynamic.

### Index Buffer

Now create the index buffer similarly to how we created the vertex buffer:

```
...

uint32_t indices[] = 
{
    0, 1, 2
};

// use the same bufferCreateInfo struct from before
bufferCreateInfo.type = Lvn_BufferType_Index;
bufferCreateInfo.usage = Lvn_BufferUsage_Static;
bufferCreateInfo.size = sizeof(indices);
bufferCreateInfo.data = indices;

LvnBuffer* indexBuffer;
lvn::createBuffer(&indexBuffer, &bufferCreateInfo);
```

Not much is different here besides changing the type to `Lvn_BufferType_Index` and the buffer size and data.

Make sure to also destroy the buffers at the end of the program once the window closes:

```
    while (lvn::windowOpen(window))
    {
        ...
    }

    lvn::destroyBuffer(vertexBuffer);
    lvn::destroyBuffer(indexBuffer);

    ...

}
```

## 4.5 Shaders and Pipelines

The next step before we can draw anything onto our window is to create a rendering pipeline for the GPU which outlines the necessary procedures for translating vertices and color onto the screen.

Before creating the pipeline, here is what the LvnPipelineCreateInfo struct contains below:

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

As you can see, the Pipeline createInfo struct requires a lot of parameters so let's go over them one at a time.

### Pipeline Specification (fixed functions)

`pipelineSpecification` is a pointer to a `LvnPipelineSpecification` struct which holds the pipeline's state or "fixed functions" such as the cull mode, depth stencil operations, viewports, multisampling, draw topology, and rasterization.

In most cases we don't need to change any of the fixed functions after the pipeline is created hence the name "fixed". To set up our pipeline specification struct, levikno provides a config function, `configPipelineSpecificationInit()`, that sets up all of the default parameters for us.

Create a `LvnPipelineSpecification` struct and a `LvnPipelineCreateInfo` below:

```
LvnPipelineSpecification pipelineSpec = lvn::configPipelineSpecificationInit();

LvnPipelineCreateInfo pipelineCreateInfo{};
pipelineCreateInfo.pipelineSpecification = &pipelineSpec;
```

You can change some of the parameters in pipelineSpec if you want to configure the rendering pipeline.

### Vertex Bindings and Attributes

Recall from the part when we created our vertex buffer, notice how the vertex data being inputed has six floats per row:

```
float vertices[] =
{
  /*   pos (x,y,z)   |  color (r,g,b)   */
    0.0f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // v1
    0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // v2
   -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // v3
};
```

For each row or vertex, we first give three float values that define the vertex's coordinate positions, then the next three floats are the color components given in RGB.

When creating the pipeline, we need to tell our pipeline how the vertex data from our buffers should be read. To do this we create a `LvnVertexAttribute` struct which looks like this:

```
struct LvnVertexAttribute
{
    uint32_t binding;
    uint32_t layout;
    LvnVertexDataType type;
    uint32_t offset;
};
```

*   `binding` is the binding number which this attribute takes its data from
*   `layout` is the shader input location number for this attribute
*   `type` is a LvnVertexDataType enum which defines the size and type of the vertex attribute data
*   `offset` is the byte offset of this attribute relative to the start of the vertex input binding

For more information on how attributes and vertex input processing works, the Vulkan docs have a detailed [guide](https://docs.vulkan.org/guide/latest/vertex_input_data_processing.html) on how it works.

Now create the attributes and add to the pipeline createInfo below:

```
LvnVertexAttribute attributes[2] =
{
    { 0, 0, Lvn_VertexDataType_Vec3f, 0 },
    { 0, 1, Lvn_VertexDataType_Vec3f, (3 * sizeof(float)) },
};

...

pipelineCreateInfo.pVertexAttributes = attributes;
pipelineCreateInfo.vertexAttributeCount = 2;
```

Note how we have two vertex attributes, one for the position and the second for the color data, both have the size and type vec3 in the shader.

Next we need to define the vertex binding descriptions, the struct for `LvnVertexBindingDescription` looks like this:

```
struct LvnVertexBindingDescription
{
    uint32_t binding;
    uint32_t stride;
};
```

*   `binding` is the binding number that this structure describes to a place in memory
*   `stride` is the length in bytes of the entire vertex (size in bytes of one vertex)

Our vertex has a stride of 6 floats (24 bytes), 3 floats for the position and 3 floats for the color data.

Add the new lines to the code below to specify the vertex binding description:

```
...

LvnVertexBindingDescription vertexBindingDescription{};
vertexBindingDescription.binding = 0;
vertexBindingDescription.stride = 6 * sizeof(float); // 3 floats for pos and 3 floats for color

...

pipelineCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
pipelineCreateInfo.vertexBindingDescriptionCount = 1;
```

### Descriptor Layouts

For now we will ignore the `pDescriptorLayouts` and descriptorLayoutCount parameters since we don't need to use them. Descriptor layouts and descriptor sets are basically blueprints to organize large chunks of data that the GPU can access through the shader such as uniform buffer data and textures.

### Shaders

The shader parameter is where we add our `LvnShader` object, in most cases, the pipeline will use two shaders, the vertex and fragment shader, for transforming vertices and computing the color data between vertices.

Create the vertex and fragment shader source codes

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

Fragment Shader:

```
#version 460

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;

void main()
{
    outColor = vec4(fragColor, 1.0);
}
```

To create shaders in levikno, levikno provides several ways to load shaders:

*   `createShaderFromSrc()` creates the shader directly from a source string
*   `createShaderFromFileSrc()` creates the shader from external files with the shader source code (eg. .vert, .frag, .glsl)
*   `createShaderFromFileBin()` creates a shader from external binary files that with the shader binary code (for Vulkan this would be a SPIR-V binary file ending in .spv)

For this case, we can just leave the shader source code within the main file and load the shaders from source.

**Note: if you are using Vulkan and the glslang libraries were not included in during building, the createShaderFromSrc() and createShaderFromFileSrc() functions will not work and will return Lvn\_Result\_Failure. You will need to load your shaders as binaries.**

```
...

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

LvnShaderCreateInfo shaderCreateInfo{};
shaderCreateInfo.vertexSrc = vertexShader;
shaderCreateInfo.fragmentSrc = fragmentShader;

LvnShader* shader;
lvn::createShaderFromSrc(&shader, &shaderCreateInfo);
```

after creating the shader, add the shader to the pipeline createInfo struct:

```
...

pipelineCreateInfo.shader = shader;
```

### Render Pass

The last parameter is the `renderPass` parameter which takes in a `LvnRenderPass` object. A render pass is used to handle different color and depth attachments used for drawing our scene. Levikno does most of the background work for us when using render passes so we only care about where our render pass will be used to render, such as to a window or to an off screen framebuffer. When we created a window, levikno automatically creates a render pass along with it. The render pass can be retrieved by calling this function:

```
LvnRenderPass* renderPass = lvn::windowGetRenderPass(window);
pipelineCreateInfo.renderPass = renderPass;
```

### Creating the Pipeline

With all the parameters set, we can now create the pipeline:

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

After the pipeline is created, if the shader is no longer needed it can be destroyed right after creating the pipeline.

## 4.6 Drawing and Main Render Loop

Drawing to the window requires a few extra functions. In the main while loop, add the following lines of code:

```
...

while (lvn::windowOpen(window))
{
    lvn::windowUpdate(window);
    lvn::windowPollEvents();

    lvn::renderBeginNextFrame(window);
    lvn::renderBeginCommandRecording(window);

    lvn::renderCmdBeginRenderPass(window, 0.0f, 0.0f, 0.0f, 1.0f);

    lvn::renderCmdBindPipeline(window, pipeline);

    lvn::renderCmdBindVertexBuffer(window, 0, 1, &vertexBuffer, 0);
    lvn::renderCmdBindIndexBuffer(window, indexBuffer, 0);

    lvn::renderCmdDrawIndexed(window, sizeof(indices) / sizeof(indices[0])); // number of elements in indices array (3)

    lvn::renderCmdEndRenderPass(window);
    lvn::renderEndCommandRecording(window);
    lvn::renderDrawSubmit(window);
}
```

Note that most render functions have the prefix render in front of the function to indicate that it is used for rendering, you will mostly use these functions only within the rendering loop.

Also note that most rendering functions will take in the window as the first parameter since the window contains most of the necessary resources for rendering such as the swapchain images, framebuffer, render passes, and surface data.

Let's break down what is happening in the main loop:

*   `renderBeginNextFrame()` begins the next window frame (in Vulkan, it acquires the next image from the swap chain). This is usually called at the beginning of a new frame or render loop.
*   `renderBeginCommandRecording()` begins command buffer recording for Vulkan under the surface. Note that rendering functions with the prefix `renderCmd` must be called only during command buffer recording.
*   `renderCmdBeginRenderPass()` begins the render pass for the window given. It then takes in four float values which are the color components in RGBA for the background color.
*   `renderCmdBindPipeline()` binds the pipeline.
*   `renderCmdBindVertexBuffer()` binds the vertex buffer, the first parameter takes in the window, the second and third parameters are the firstBinding and bindingCount, here the binding is set to 0 and the count set to 1 because we only have one binding. The forth and fifth parameters are pBuffers and pOffsets which takes in an array of buffers and offsets if we have more than one binding.
*   `renderCmdBindIndexBuffer()` binds the index buffer, the window and indexBuffer is passed in for the first and second parameters. The third parameter is an offset value which can be used if the index data and the vertex data are stored within the same buffer.
*   `renderCmdDrawIndexed()` will draw the vertices using the index data from our buffers. Because our triangle only has three indices to draw three vertices, we pass 3 into our function.
*   `renderCmdEndRenderPass()` and `renderEndCommandRecording()` ends the render pass and command buffer respectively
*   `renderDrawSubmit()` submits the draw commands from the command buffers and sends them to the GPU to render. It then presents the drawn data to the window after the GPU finishes rendering

Remember to destroy all of the object created when before terminating the context and exiting the program:

```
...

// we destroyed our shader earlier after creating the pipeline

lvn::destroyBuffer(vertexBuffer);
lvn::destroyBuffer(indexBuffer);
lvn::destroyPipeline(pipeline);
lvn::destroyWindow(window);

lvn::terminateContext();

return 0;
```

Now compile and run the program, a triangle should appear on the window.

<p align="center">
    <img src="https://github.com/binmian/Levikno/blob/main/.github/simpleTriangleExample.png">
</p>

You can also check [here](https://github.com/binmian/levikno/blob/main/examples/simpleTriangle.cpp) to see the full source code for this example.
