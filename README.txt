--> https://vulkan-tutorial.com/en/Overview

What it takes to draw a triangle?
    - Step 1: Vulkan Instance and Physical device selection
        VkInstance
        VkPhysicalDevice
    - Step 2: Logical Device and Queue Families
        Create a logical device VkDevice by selecting which features you are
        interested in on the actual Physical device
        Once selected we would need to specify which queue families are needed
    - Step 3: Window surface and swap chain
        Creating a window to recieve the data throgh VkSurfaceKHR
        The swap chain is a collection of render targets, the swap chain will
        provide the image to render to and will be in charge of putting it in the
        screen
    - Step 4: Image Views and Framebuffers
        Wrap to VkImageView and VkFrameBuffer
            - VkImageView:
                This is a handler for an image object containing metadata and properties
            - VkFrameBuffer:
                This is used by render passes
    - Step 5: Render Passes
        Describe the type of images that are used during rendering operations
        VkBuffers does the binding of these images to the slots
    - Step 6: Graphics Pipeline
        Create and configure the VkPipeline object through VkShaderModule
    - Step 7: 
        
        

extensions
    'KHR' suffix denotes that the class is part of a Vulkan Extension
        VkSurfaceKHR
            Handles windows in a platform-agnostic way
shaders
    --> https://en.wikipedia.org/wiki/Shader

🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟🐟
review 2025-03-01
New Vulkan Tutorial
    --> https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/00_Base_code.html
Vulkan C++ bindings
    --> https://github.com/KhronosGroup/Vulkan-Hpp

Origin of Vulkan
    "back then, programmers had to provide the 'vertex data' in a standard format
    and they were at the mercy of the GPU manufacturers with regards to lighten
    and shading options"

    **What exactly is 'vertex data', let alone 'vertex'
    
    Syntax for 'shaders' is one example o inconsistencies between vendors

    **What exactly is a 'shader'

    Vulkan = Graphics + Compute functionalities

What is a shader?
    --> https://en.wikipedia.org/wiki/Shader
    --> ai
    The concept of 'shader', refers to any type of program in the 'pipeline' that
    is responsible for processing graphical data

    There are a bunch of shader types:
        - Vertex Shader
            Handles Transformations (Object Space to World Space, Camera Space)
            Runs once in every vertex
        - Fragment Shader (Pixel Shader)
        - Geometry Shader
        - Tessellation Shaders
        - Compute Shader

Host
    PhyDevs
        Queue Families
            *Queues

WSI -> Window System Interface
    It introduces the VkSurfaceKHR and the VkSwapchainKHR
    Window Surface
    Swapchain
        Collection of render targets
        Its purpose is to ensure that the image we are gurrently rendering
        is different from what is currently on screen
        Whenever we want to draw a frame we have to ask the swap chain to provide
        us with an image to render to
        When we are finished drawing a frame, the image is returned to the
        swapchain so it gets rendered to the screen

        The number of render targets depends on the 'present mode'
        'Present modes' are either vsynk (double buffered), or triple buffered

        Workflow
            - Ask the swapchain for an image
            - Draw on the image
            - Give the swapchain the image back
            - The swapchain renders it on screen

Image Views and Framebuffers
    To draw an image from the swapchain we have to wrap it in a VkImageView and
    VkFrameBuffer

    ImageView
        References an specific part of an image to be used
    Framebuffer
        References image views that are to be used for color/depth/stencil targets
        Since there might be a lot of images in the swapchain we prepare ourselves
        by creating an image view and framebuffer pair for each of them
        (Preemtively creating them)

Render Passes
    They describe the 'type' of images that are used during rendering operations
    How will they be used and how their contents should be treated

    The render pass only describes the type of images, a FrameBuffer actually
    binds specific images to these slots

Graphics Pipeline
    Describes the configurable state of the graphics card

Command Pools and Command Buffers
    Commands are sent to a queue to be excecuted in the device (GPU)

    These commands are recorded into command buffers, 
    the command buffers in turn are allocated from Command Pools
    The Command Pools are associated with a given queue family in the device

        Command Buffers  --- allocated from -->  Command Pools --> Queue Family --> Device --> Physical Device

    Drawing a triangle, the command buffer would look like this:
        - Begin the render pass
        - Bind the graphics pipeline 
        - Draw 3 vertices
        - End the render pass

    We need to create an specific command buffer per each possible image type, and
    select the correct frame buffer once we get it from the swapchain

Main Loop
    Acquire image from the swapchain (vkAcquireNextImageKHR)
    Select appropiate command buffer for that image
    Submit command buffer to the queue
    Return the image to the swapchain for presentation (vkQueuePresentKHR)
    
    Operations submitted to queues are executed asynch
    We need to use synch objects to ensure a correct order of execution

Cheaty cheato
    Embedding vertex coordinates instead of using a vertex buffer

Summary Steps
    - Create a VkInstance
    - Select supported VkPhysicalDevice
    - Create a VkDevice and VkQueue for drawing and presentation
    - Create a window, window surface and swap chain
    - Wrap the swapchain images into VkImageView
    - Create a render pass that specifies the render targets and usage
    - Create framebuffers for the render pass
    - Set up the graphics pipeline
    - Allocate and record a command buffer with the draw commands for every possible
      swap chain image
    - Draw frames by acquiring images, submitting the right draw command buffer, 
      returning the images back to the swapchain

MacOS Specific
    MacOS does not support vulkan natively, MoltenVK, which is what the Vulkan SDK
    uses internally, translates API calls to Metal, which in fact is native
CONCEPTS
    Drawing vs Rendering
        Drawing actually asks the GPU to draw something
    
Resource management
    RAII is the adviced approach for bigger vulkan programs


Queues & Devices review 2025-03-04
    Each Device can contain a number of Queue Families, say
        Dev{Qf1, Qf2, Qf3, ... Qfn}
    Queue families are characterized by their capabilities, these
    are represented by a flag parameter, also each queue family has
    a number of queues that support such family say
        There are 3 queues in Qf1 which have the flag of 0xff

    The way vulkan retrieves this information is by querying the flags,
    and queue family by the index of an array vulkan retrieves to you
        vkGetPhysicalDeviceQueueFamilyProperties

    Once we identify the wanted queue type, we store the index of the
    Queue Family and then create the logical queue while creating the
    logical device

    **How are queues shared accross vulkan instances?
    **Can I requests more queues than the availables within a family?

Queue submission
    vkQueueSubmit submits 'work' to the queue, they define a set of queue
    operations to be executed by the underlying physical device

Command Buffers
    There are two types
        Primary command buffers
            They execute secondary command buffers
            They are direclty submitted to the queue
            Command buffer state
                Initial
                Recording
                Executable
                Pending

Command Pools

REDESIGN 2025-03-10
    VulkanInstance
        PhysicalDevices
            QueueFamilies
    Host
        -> VulkanInstance
        -> PhysicalDevices

IMPLEMENTATION
    Framebuffer
        --> https://en.wikipedia.org/wiki/Framebuffer
        Portion of RAM containing bitmaps that drives a video display
        It is part of the RAM in a program that is used by the application
        for representation of the content to be shown on the display

    Swapchain
        --> Spec 34.5 
        It is a queue of messages that are waiting to be presented to the screen
        The application 'acquires' the image, draws on it, and returns it

        The main purpose is to synchronize the presentation of images with the
        refresh rate of the screen

        A physical device might or might not support the swapchain, this is platform
        dependent and hence it is provided as a device-extension

        We also need to check for the following properties:
            - Surface capabilities
                - min/max number of images in the swapchain
                - min/max width and height of images
            - Surface formats
                - pixel format
                - color space
            - Available presentation modes

        The capabilities of the Swapchain targeting a surface are the intersection of the
        following:
            - WSI platform
            - Native window or display
            - Physical device

        SwapChain
            -> PhysicalDevice
            -> SurfaceKHR
            -> 
TODO

MoltenVK
    --> https://vulkan.lunarg.com/doc/sdk/latest/mac/getting_started.html
    Options
        - Link directly to MoltenVK (either statically or dynamically)
            Allows mixed used of vulkan and metal
        - XCFramework static library
    Recommended option
        Use MoltenVK as dynamic lib and the Vulkan Loader
            MoltenVK will be threated as an ICD by the Vulkan Loader
        In this mode we would only need to link agains the loader, not MoltenVk
        We will still need ot include the MoltenVK and Loader's dynamic libraries when shipping

GLFW
    --> https://www.glfw.org/docs/latest/vulkan_guide.html

    Issue is that I am not able to create a VkSurfaceKHR out of the glfw window for some reason
    We are getting a -7 VkResult
        VK_ERROR_EXTENSION_NOT_PRESENT = -7,
    GLFW triangle sample app is also not being successfully created

