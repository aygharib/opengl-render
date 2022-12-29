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
auto createGLFWWindow() -> GLFWwindow* {
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME,
        NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
    }

    // Context is set to created window on main thread
    glfwMakeContextCurrent(window);

    return window;
}

// Initialize OpenGL function pointers using GLAD before calling any OpenGL functions
auto initGLAD() -> void {
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
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

auto createVertexShader() -> unsigned int {
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return vertexShader;
}

auto createFragmentShader() -> unsigned int {
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
        "}\n\0";
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    int success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return fragmentShader;
}

auto createShader() -> unsigned int {
    auto vertexShader = createVertexShader();
    auto fragmentShader = createFragmentShader();

    unsigned int shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);

    int success;
    char infoLog[512];
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINKING::FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shader;
}

auto createVAO() -> unsigned int {
    // Triangle vertices in normalized device coordinates
    // Vertex data to be sent to the vertex shader via a VBO
    float vertices[] = {
        // X, Y, Z positions
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // Bind VAO before creation on VBO to store calls to
    // glVertexAttribPointer, glEnableVertexAttribArray
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create and bind temporary VBO to GL_ARRAY_BUFFER buffer to train VAO
    unsigned int tempVBO;
    glGenBuffers(1, &tempVBO);
    glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
    
    // Supply vertex data to whichever buffer is in GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configure vertex attributes on how to process vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    // Unbind vertex array to prevent future VertexAttrib calls from modifying VAO
    glBindVertexArray(0);

    // Remove temporary VBO
    glDeleteBuffers(1, &tempVBO);

    return VAO;
}

auto deallocateResources(unsigned int VAO, unsigned int shader) -> void {
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shader);
}

int main() {
    initGLFW();
    auto window = createGLFWWindow();
    initGLAD();
    setCallbackFunctions(window);

    auto shader = createShader();
    glUseProgram(shader);

    auto VAO = createVAO();

    while(!glfwWindowShouldClose(window)) {
        processWindowInput(window);

        // Overwrites previous render screen with blank color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap front and back GLFW color buffers after render commands are complete
        // so the image is displayed without still being rendered to, to remove any
        // render artifacts
        glfwSwapBuffers(window);

        // Poll input-output events (keyboard, mouse, etc) to update window state and
        // call corresponding registered callback functions
        glfwPollEvents();
    }

    deallocateResources(VAO, shader);

    // Before application terminates, GLFW should be terminated to free any resources
    // allocated during or after GLFW initialization
    glfwTerminate();
    
    return 0;
}