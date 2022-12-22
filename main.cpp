#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource="#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

    // Input
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, //left
        0.5f, -0.5f, 0.0f,  //right
        0.0f,  0.5f, 0.0f   //top
    };
    // Vertex Buffer object
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check if vertex shader compiles
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment shader    
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check if fragment shader compiles
    int success2;
    char infoLog2[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success2);
    if(!success2) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog2);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog2 << std::endl;
    }

    // Shader program
    // to use the compiled shaders above, the shader program must be activated when rendering objects
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Delete shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Check if linking successful
    int success3;
    char infoLog3[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success3);
    if(!success3) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog3);
        std::cout << "ERROR::LINKING_FAILED\n" << infoLog3 << std::endl;
    }

    // Activate program object
    glUseProgram(shaderProgram);

    // Tell OpenGL how to interpret the vertex data to map to vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Enable the vertex attribute at location 0
    glEnableVertexAttribArray(0);

    // Vertex Array Object (VAO)
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // ..:: Initialization code (done once (unless your object frequently changes)) :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Render loop
    while(!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // GLFW swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    // Clean all GLFW resources that were allocated
    glfwTerminate();
    return 0;
}