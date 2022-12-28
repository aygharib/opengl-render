#include <cmath>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr int WINDOW_WIDTH    = 800;
constexpr int WINDOW_HEIGHT   = 600;
constexpr char WINDOW_NAME[6] = { 'R', 'e', 'n', 'd', 'e', 'r' };

// GLFW library must be initialized before most GLFW functions can be used
auto initGLFW() -> void {
    glfwInit();

    // OpenGL version 3.3 as it's the first version thats considered "Modern OpenGL"
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Use Core Profile to remove deprecated features
    // Remove backwards compatibility from older OpenGL versions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

// Create GLFW window and set it as current context
// GLFW window required
auto createGLFWWindow() -> GLFWwindow* {
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME,
        NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    // Context is set to created window
    glfwMakeContextCurrent(window);

    return window;
}

// Initialize GLAD before OpenGL functions
auto initGLAD() -> void {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
}

// Update the size of rendering window (viewport) to match new GLFWwindow size
// so OpenGL knows how to display data and coordinates with respect to the window
auto framebuffer_size_callback(GLFWwindow* window, int width, int height) -> void {
    glViewport(0, 0, width, height);
}

auto setCallbackFunctions(GLFWwindow* window) -> void {
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

auto processWindowInput(GLFWwindow *window) -> void {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

int main() {
    initGLFW();
    auto window = createGLFWWindow();
    initGLAD();
    setCallbackFunctions(window);

    while(!glfwWindowShouldClose(window)) {
        processWindowInput(window);

        // Overwrites previous render screen with blank color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap front and back GLFW color buffers after render commands are complete
        // so the image is displayed without still being rendered to, to remove any
        // render artifacts
        glfwSwapBuffers(window);

        // Poll input-output events (keyboard, mouse, etc) to update window state and
        // call corresponding registered callback functions
        glfwPollEvents();
    }

    // Before application terminates, GLFW should be terminated to free any resources
    // allocated during or after GLFW initialization
    glfwTerminate();
    
    return 0;
}