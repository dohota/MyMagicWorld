package javaworld.rendering;

import org.lwjgl.glfw.GLFW;

import static org.lwjgl.glfw.GLFW.*;

public class Window {
    private final int width;
    private final int height;
    private final String title;

    private long handle;

    public Window(int width, int height, String title) {
        this.width = width;
        this.height = height;
        this.title = title;
    }

    public void create() {
        if (!glfwInit()) {
            throw new IllegalStateException("Unable to initialize GLFW");
        }

        // if (!GLFW.glfwVulkanSupported()) {
        //     throw new RuntimeException("Vulkan is not supported on this machine.");
        // }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        handle = glfwCreateWindow(width, height, title, 0, 0);

        if (handle == 0L) {
            throw new RuntimeException("Failed to create GLFW window.");
        }
    }

    public boolean shouldClose() {
        return glfwWindowShouldClose(handle);
    }

    public void pollEvents() {
        glfwPollEvents();
    }

    public long getHandle() {
        return handle;
    }

    public void cleanup() {
        if (handle != 0L) {
            glfwDestroyWindow(handle);
            handle = 0L;
        }

        glfwTerminate();
    }
}