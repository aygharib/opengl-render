#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    // Initialize GLFW, set core-profile and OpenGL version
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create GLFW window and set it as current context
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD before OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set size of render viewport, mapped to the window
    // 0, 0 is bottom left corner coordinate
    glViewport(0, 0, 800, 600);

    // Set callback function when resizing window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Render loop
    while(!glfwWindowShouldClose(window)) {
        // Swap between front and back buffer
        glfwSwapBuffers(window);

        // Check if any events are triggered, updates window state, and calls corresponding functions
        glfwPollEvents();    
    }

    // Clean all GLFW resources that were allocated
    glfwTerminate();
    return 0;
}