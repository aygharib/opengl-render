// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cmath>
#include <iostream>
#include "Camera.h"
#include "Shader.h"

namespace {
float last_mouse_x{400};
float last_mouse_y{300};
bool first_mouse{true};
Camera camera{glm::vec3{0.F, 0.F, 3.F}};

glm::vec3 cube_positions[] = {glm::vec3(0.F, 0.F, 0.F),
                              glm::vec3(2.F, 5.F, -15.F),
                              glm::vec3(-1.5F, -2.2F, -2.5F),
                              glm::vec3(-3.8F, -2.F, -12.3F),
                              glm::vec3(2.4F, -0.4F, -3.5F),
                              glm::vec3(-1.7F, 3.F, -7.5F),
                              glm::vec3(1.3F, -2.F, -2.5F),
                              glm::vec3(1.5F, 2.F, -2.5F),
                              glm::vec3(1.5F, 0.2F, -1.5F),
                              glm::vec3(-1.3F, 1.F, -1.5F)};

auto framebuffer_size_callback(GLFWwindow* window, int width, int height) -> void {
    glViewport(0, 0, width, height);
}

auto mouse_callback(GLFWwindow* window, double xpos, double ypos) -> void {
    if (first_mouse) {
        last_mouse_x = xpos;
        last_mouse_y = ypos;
        first_mouse  = false;
    }

    const auto xoffset{xpos - last_mouse_x};
    const auto yoffset{last_mouse_y - ypos};
    last_mouse_x = xpos;
    last_mouse_y = ypos;

    camera.process_mouse_movement(xoffset, yoffset);
}

auto scroll_callback(GLFWwindow* window, double xoffset, double yoffset) -> void {
    camera.process_mouse_scroll(static_cast<float>(yoffset));
}

auto process_input(GLFWwindow* window, float deltaTime) -> void {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, 1);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.process_keyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.process_keyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.process_keyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.process_keyboard(Camera_Movement::RIGHT, deltaTime);
}

auto init_GLFW() -> void {
    // Initialize GLFW, set core-profile and OpenGL version
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

// Create GLFW window and set it as current context
auto create_window() -> GLFWwindow* {
    GLFWwindow* window{glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr)};
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    return window;
}

// Initialize GLAD before OpenGL functions
auto init_GLAD() -> void {
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
}

auto set_callback_functions(GLFWwindow* window) -> void {
    // Set callback function when resizing window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // hide the cursor and capture it
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set mouse callback
    glfwSetCursorPosCallback(window, mouse_callback);

    // Set scroll callback
    glfwSetScrollCallback(window, scroll_callback);
}

auto create_VAO() -> unsigned int {
    // Input vertices data
    float vertices[] = {
      // position           // texture
      -0.5F, -0.5F, -0.5F, 0.F, 0.F, 0.5F,  -0.5F, -0.5F, 1.F, 0.F, 0.5F,  0.5F,  -0.5F, 1.F, 1.F,
      0.5F,  0.5F,  -0.5F, 1.F, 1.F, -0.5F, 0.5F,  -0.5F, 0.F, 1.F, -0.5F, -0.5F, -0.5F, 0.F, 0.F,

      -0.5F, -0.5F, 0.5F,  0.F, 0.F, 0.5F,  -0.5F, 0.5F,  1.F, 0.F, 0.5F,  0.5F,  0.5F,  1.F, 1.F,
      0.5F,  0.5F,  0.5F,  1.F, 1.F, -0.5F, 0.5F,  0.5F,  0.F, 1.F, -0.5F, -0.5F, 0.5F,  0.F, 0.F,

      -0.5F, 0.5F,  0.5F,  1.F, 0.F, -0.5F, 0.5F,  -0.5F, 1.F, 1.F, -0.5F, -0.5F, -0.5F, 0.F, 1.F,
      -0.5F, -0.5F, -0.5F, 0.F, 1.F, -0.5F, -0.5F, 0.5F,  0.F, 0.F, -0.5F, 0.5F,  0.5F,  1.F, 0.F,

      0.5F,  0.5F,  0.5F,  1.F, 0.F, 0.5F,  0.5F,  -0.5F, 1.F, 1.F, 0.5F,  -0.5F, -0.5F, 0.F, 1.F,
      0.5F,  -0.5F, -0.5F, 0.F, 1.F, 0.5F,  -0.5F, 0.5F,  0.F, 0.F, 0.5F,  0.5F,  0.5F,  1.F, 0.F,

      -0.5F, -0.5F, -0.5F, 0.F, 1.F, 0.5F,  -0.5F, -0.5F, 1.F, 1.F, 0.5F,  -0.5F, 0.5F,  1.F, 0.F,
      0.5F,  -0.5F, 0.5F,  1.F, 0.F, -0.5F, -0.5F, 0.5F,  0.F, 0.F, -0.5F, -0.5F, -0.5F, 0.F, 1.F,

      -0.5F, 0.5F,  -0.5F, 0.F, 1.F, 0.5F,  0.5F,  -0.5F, 1.F, 1.F, 0.5F,  0.5F,  0.5F,  1.F, 0.F,
      0.5F,  0.5F,  0.5F,  1.F, 0.F, -0.5F, 0.5F,  0.5F,  0.F, 0.F, -0.5F, 0.5F,  -0.5F, 0.F, 1.F};

    unsigned int indices[] = {
      // note that we start from 0!
      0,
      1,
      3,  // first triangle
      1,
      2,
      3  // second triangle
    };

    // Vertex Buffer object
    unsigned int VAO{0};
    unsigned int VBO{0};
    unsigned int EBO{0};
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind Vertex Array Object
    glBindVertexArray(VAO);
    // Copy vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return VAO;
}

auto render(Shader& shaderProgram, unsigned int vao, unsigned int texture1, unsigned int texture2)
  -> void {
    glClearColor(0.2F, 0.3F, 0.3F, 1.F);
    // clear depth buffer before each render to clear previous depth data (similar to clearing
    // previous color buffer)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);  // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);  // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, texture2);

    shaderProgram.use();

    // create View matrix
    auto view{camera.get_view_matrix()};

    // create Projection matrix
    auto projection{glm::perspective(glm::radians(camera.get_zoom()), 800.F / 600.F, 0.1F, 100.F)};

    /// Send matrices to shader (this is usually done each frame since transformation matrices tend
    /// to change a lot)
    shaderProgram.set_mat4("view", view);
    shaderProgram.set_mat4("projection", projection);

    // Draw square
    glBindVertexArray(vao);
    for (auto i{0}; i < 10; i++) {
        // create Model matrix
        auto model{glm::mat4(1.F)};
        model = glm::translate(model, cube_positions[i]);
        const float angle{20.F * static_cast<float>(i)};
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.F, 0.3F, 0.5F));
        shaderProgram.set_mat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

auto create_texture(const char* path, bool isPNG) -> unsigned int {
    unsigned int texture{0};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width{0};
    int height{0};
    int nrChannels{0};
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data != nullptr) {
        if (isPNG) {
            glTexImage2D(
              GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        } else {
            glTexImage2D(
              GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    std::cout << path << " loaded\n";
    return texture;
}
}  // namespace

auto main() -> int {
    init_GLFW();
    auto* window = create_window();
    init_GLAD();
    set_callback_functions(window);

    Shader shader_program("/home/wumbo/dev/opengl-by-example/shaders/shader.vs",
                          "/home/wumbo/dev/opengl-by-example/shaders/shader.fs");
    auto VAO{create_VAO()};

    stbi_set_flip_vertically_on_load(1);
    const auto texture1{
      create_texture("/home/wumbo/dev/opengl-by-example/resources/textures/container.jpg", false)};
    const auto texture2{create_texture(
      "/home/wumbo/dev/opengl-by-example/resources/textures/grunge-scratch.png", true)};

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    shader_program.use();
    shader_program.set_int("texture1", 0);  // set it manually
    shader_program.set_int("texture2", 1);  // or with shader class

    glEnable(GL_DEPTH_TEST);

    float delta_time{0.F};  // Time between current frame and last frame
    float last_frame{0.F};  // Time of last frame

    while (glfwWindowShouldClose(window) == 0) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        delta_time        = currentFrame - last_frame;
        last_frame        = currentFrame;

        process_input(window, delta_time);

        render(shader_program, VAO, texture1, texture2);

        // GLFW swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean all GLFW resources that were allocated
    glfwTerminate();
    return 0;
}