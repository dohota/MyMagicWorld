package javaworld.rendering;

import org.lwjgl.PointerBuffer;
import org.lwjgl.glfw.GLFWVulkan;
import org.lwjgl.system.MemoryStack;
import org.lwjgl.vulkan.*;

import java.nio.IntBuffer;
import java.nio.LongBuffer;
import java.util.HashSet;
import java.util.Set;

import static org.lwjgl.glfw.GLFWVulkan.glfwCreateWindowSurface;
import static org.lwjgl.glfw.GLFWVulkan.glfwGetRequiredInstanceExtensions;
import static org.lwjgl.system.MemoryStack.stackPush;
import static org.lwjgl.system.MemoryUtil.NULL;
import static org.lwjgl.vulkan.EXTDebugUtils.VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
import static org.lwjgl.vulkan.KHRPortabilityEnumeration.VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
import static org.lwjgl.vulkan.KHRPortabilityEnumeration.VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
import static org.lwjgl.vulkan.KHRPortabilitySubset.VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME;
import static org.lwjgl.vulkan.KHRSurface.*;
import static org.lwjgl.vulkan.VK10.*;

public class VulkanRenderer {
    private final Window window;

    private VkInstance instance;
    private long surface;
    private VkPhysicalDevice physicalDevice;
    private VkDevice device;
    private VkQueue graphicsQueue;
    private VkQueue presentQueue;

    private int graphicsQueueFamilyIndex = -1;
    private int presentQueueFamilyIndex = -1;

    public VulkanRenderer(Window window) {
        this.window = window;
    }

    public void init() {
        createInstance();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();

        System.out.println("Vulkan initialized successfully.");
    }

    private void createInstance() {
    try (MemoryStack stack = stackPush()) {
        VkApplicationInfo appInfo = VkApplicationInfo.calloc(stack)
                .sType(VK_STRUCTURE_TYPE_APPLICATION_INFO)
                .pApplicationName(stack.UTF8("LWJGL Vulkan Demo"))
                .applicationVersion(VK_MAKE_VERSION(1, 0, 0))
                .pEngineName(stack.UTF8("JavaWorld Engine"))
                .engineVersion(VK_MAKE_VERSION(1, 0, 0))
                .apiVersion(VK_API_VERSION_1_0);

        PointerBuffer glfwExtensions = glfwGetRequiredInstanceExtensions();

        if (glfwExtensions == null) {
            throw new RuntimeException("Failed to get required GLFW Vulkan extensions.");
        }

        boolean portabilitySupported =
                isInstanceExtensionSupported(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        int extraExtensionCount = portabilitySupported ? 1 : 0;

        PointerBuffer extensions =
                stack.mallocPointer(glfwExtensions.remaining() + extraExtensionCount);

        for (int i = 0; i < glfwExtensions.remaining(); i++) {
            extensions.put(glfwExtensions.get(i));
        }

        int flags = 0;

        if (portabilitySupported) {
            extensions.put(stack.UTF8(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME));
            flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }

        extensions.flip();

        VkInstanceCreateInfo createInfo = VkInstanceCreateInfo.calloc(stack)
                .sType(VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO)
                .pApplicationInfo(appInfo)
                .ppEnabledExtensionNames(extensions)
                .flags(flags);

        PointerBuffer instancePointer = stack.mallocPointer(1);

        int result = vkCreateInstance(createInfo, null, instancePointer);

        if (result != VK_SUCCESS) {
            throw new RuntimeException("Failed to create Vulkan instance. Error code: " + result);
        }

        instance = new VkInstance(instancePointer.get(0), createInfo);
    }
}

    private void createSurface() {
        try (MemoryStack stack = stackPush()) {
            LongBuffer surfacePointer = stack.mallocLong(1);

            int result = glfwCreateWindowSurface(instance, window.getHandle(), null, surfacePointer);

            if (result != VK_SUCCESS) {
                throw new RuntimeException("Failed to create Vulkan surface. Error code: " + result);
            }

            surface = surfacePointer.get(0);
        }
    }

    private void pickPhysicalDevice() {
        try (MemoryStack stack = stackPush()) {
            IntBuffer deviceCount = stack.ints(0);

            vkEnumeratePhysicalDevices(instance, deviceCount, null);

            if (deviceCount.get(0) == 0) {
                throw new RuntimeException("No Vulkan-capable GPU found.");
            }

            PointerBuffer devices = stack.mallocPointer(deviceCount.get(0));
            vkEnumeratePhysicalDevices(instance, deviceCount, devices);

            for (int i = 0; i < devices.capacity(); i++) {
                VkPhysicalDevice candidate = new VkPhysicalDevice(devices.get(i), instance);

                if (isDeviceSuitable(candidate)) {
                    physicalDevice = candidate;
                    return;
                }
            }

            throw new RuntimeException("No suitable Vulkan physical device found.");
        }
    }

    private boolean isDeviceSuitable(VkPhysicalDevice device) {
        findQueueFamilies(device);
        return graphicsQueueFamilyIndex >= 0 && presentQueueFamilyIndex >= 0;
    }

    private void findQueueFamilies(VkPhysicalDevice device) {
        try (MemoryStack stack = stackPush()) {
            IntBuffer queueFamilyCount = stack.ints(0);

            vkGetPhysicalDeviceQueueFamilyProperties(device, queueFamilyCount, null);

            VkQueueFamilyProperties.Buffer queueFamilies =
                    VkQueueFamilyProperties.malloc(queueFamilyCount.get(0), stack);

            vkGetPhysicalDeviceQueueFamilyProperties(device, queueFamilyCount, queueFamilies);

            for (int i = 0; i < queueFamilies.capacity(); i++) {
                VkQueueFamilyProperties queueFamily = queueFamilies.get(i);

                if ((queueFamily.queueFlags() & VK_QUEUE_GRAPHICS_BIT) != 0) {
                    graphicsQueueFamilyIndex = i;
                }

                IntBuffer presentSupport = stack.ints(VK_FALSE);
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, presentSupport);

                if (presentSupport.get(0) == VK_TRUE) {
                    presentQueueFamilyIndex = i;
                }

                if (graphicsQueueFamilyIndex >= 0 && presentQueueFamilyIndex >= 0) {
                    return;
                }
            }
        }
    }

    private void createLogicalDevice() {
        try (MemoryStack stack = stackPush()) {
            Set<Integer> uniqueQueueFamilies = new HashSet<>();
            uniqueQueueFamilies.add(graphicsQueueFamilyIndex);
            uniqueQueueFamilies.add(presentQueueFamilyIndex);

            VkDeviceQueueCreateInfo.Buffer queueCreateInfos =
                    VkDeviceQueueCreateInfo.calloc(uniqueQueueFamilies.size(), stack);

            int index = 0;

            for (int queueFamily : uniqueQueueFamilies) {
                queueCreateInfos.get(index)
                        .sType(VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO)
                        .queueFamilyIndex(queueFamily)
                        .pQueuePriorities(stack.floats(1.0f));

                index++;
            }

            PointerBuffer deviceExtensions = getRequiredDeviceExtensions(stack);

            VkPhysicalDeviceFeatures deviceFeatures = VkPhysicalDeviceFeatures.calloc(stack);

            VkDeviceCreateInfo createInfo = VkDeviceCreateInfo.calloc(stack)
                    .sType(VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO)
                    .pQueueCreateInfos(queueCreateInfos)
                    .pEnabledFeatures(deviceFeatures)
                    .ppEnabledExtensionNames(deviceExtensions);

            PointerBuffer devicePointer = stack.mallocPointer(1);

            int result = vkCreateDevice(physicalDevice, createInfo, null, devicePointer);

            if (result != VK_SUCCESS) {
                throw new RuntimeException("Failed to create Vulkan logical device. Error code: " + result);
            }

            device = new VkDevice(devicePointer.get(0), physicalDevice, createInfo);

            PointerBuffer queuePointer = stack.mallocPointer(1);

            vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, queuePointer);
            graphicsQueue = new VkQueue(queuePointer.get(0), device);

            vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, queuePointer);
            presentQueue = new VkQueue(queuePointer.get(0), device);
        }
    }

    private PointerBuffer getRequiredDeviceExtensions(MemoryStack stack) {
        Set<String> supportedExtensions = getSupportedDeviceExtensions();

        boolean supportsPortabilitySubset =
                supportedExtensions.contains(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);

        PointerBuffer extensions = stack.mallocPointer(supportsPortabilitySubset ? 1 : 0);

        if (supportsPortabilitySubset) {
            extensions.put(stack.UTF8(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME));
        }

        extensions.flip();
        return extensions;
    }

    private Set<String> getSupportedDeviceExtensions() {
        try (MemoryStack stack = stackPush()) {
            IntBuffer extensionCount = stack.ints(0);

            vkEnumerateDeviceExtensionProperties(physicalDevice, (String) null, extensionCount, null);

            VkExtensionProperties.Buffer availableExtensions =
                    VkExtensionProperties.malloc(extensionCount.get(0), stack);

            vkEnumerateDeviceExtensionProperties(
                    physicalDevice,
                    (String) null,
                    extensionCount,
                    availableExtensions
            );

            Set<String> extensions = new HashSet<>();

            for (int i = 0; i < availableExtensions.capacity(); i++) {
                extensions.add(availableExtensions.get(i).extensionNameString());
            }

            return extensions;
        }
    }
    private boolean isInstanceExtensionSupported(String extensionName) {
    try (MemoryStack stack = stackPush()) {
        IntBuffer extensionCount = stack.ints(0);

        vkEnumerateInstanceExtensionProperties((String) null, extensionCount, null);

        VkExtensionProperties.Buffer availableExtensions =
                VkExtensionProperties.malloc(extensionCount.get(0), stack);

        vkEnumerateInstanceExtensionProperties(
                (String) null,
                extensionCount,
                availableExtensions
        );

        for (int i = 0; i < availableExtensions.capacity(); i++) {
            if (availableExtensions.get(i).extensionNameString().equals(extensionName)) {
                return true;
            }
        }

        return false;
    }
}

    public void cleanup() {
        if (device != null) {
            vkDeviceWaitIdle(device);
            vkDestroyDevice(device, null);
            device = null;
        }

        if (surface != NULL) {
            vkDestroySurfaceKHR(instance, surface, null);
            surface = NULL;
        }

        if (instance != null) {
            vkDestroyInstance(instance, null);
            instance = null;
        }
    }
}