package javaworld;

import javaworld.rendering.VulkanRenderer;
import javaworld.rendering.Window;

public class App {
    public static void main(String[] args) {
        Window window = new Window(800, 600, "LWJGL + Vulkan Demo");
        VulkanRenderer renderer = new VulkanRenderer(window);

        try {
            window.create();
            renderer.init();

            while (!window.shouldClose()) {
                window.pollEvents();
            }

            renderer.cleanup();
            window.cleanup();
        } catch (RuntimeException e) {
            renderer.cleanup();
            window.cleanup();
            throw e;
        }
    }
}