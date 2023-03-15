// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fmt/core.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <utility>
#include <vector>
#include "Camera.h"
#include "Shader.h"

auto cube_positions = std::array<glm::vec3, 10>{glm::vec3{0.F, 0.F, 0.F},
                                                glm::vec3{2.F, 5.F, -15.F},
                                                glm::vec3{-1.5F, -2.2F, -2.5F},
                                                glm::vec3{-3.8F, -2.F, -12.3F},
                                                glm::vec3{2.4F, -0.4F, -3.5F},
                                                glm::vec3{-1.7F, 3.F, -7.5F},
                                                glm::vec3{1.3F, -2.F, -2.5F},
                                                glm::vec3{1.5F, 2.F, -2.5F},
                                                glm::vec3{1.5F, 0.2F, -1.5F},
                                                glm::vec3{-1.3F, 1.F, -1.5F}};

namespace {
auto last_mouse_x = 400.F;
auto last_mouse_y = 300.F;
auto first_mouse  = true;
auto camera       = Camera{glm::vec3{0.F, 0.F, 3.F}};

auto framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height) -> void {
    glViewport(0, 0, width, height);
}

auto mouse_callback(GLFWwindow* /*window*/, double xpos, double ypos) -> void {
    if (first_mouse) {
        last_mouse_x = static_cast<float>(xpos);
        last_mouse_y = static_cast<float>(ypos);
        first_mouse  = false;
    }

    const auto xoffset = xpos - last_mouse_x;
    const auto yoffset = last_mouse_y - ypos;
    last_mouse_x       = static_cast<float>(xpos);
    last_mouse_y       = static_cast<float>(ypos);

    camera.process_mouse_movement(xoffset, yoffset);
}

auto scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset) -> void {
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
    auto* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        fmt::print("Failed to create GLFW window\n");
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    return window;
}

// Initialize GLAD before OpenGL functions
auto init_GLAD() -> void {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
        fmt::print("Failed to initialize GLAD\n");
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
    // First 3 are position, next 3 are texture, for each vertex
    auto vertices = std::array<float, 180>{
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

    auto indices = std::array<int, 6>{// first triangle
                                      0,
                                      1,
                                      3,
                                      // second triangle
                                      1,
                                      2,
                                      3};

    // Vertex Buffer object
    auto VAO = unsigned{0};
    auto VBO = unsigned{0};
    auto EBO = unsigned{0};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind Vertex Array Object
    glBindVertexArray(VAO);
    // Copy vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), static_cast<void*>(&vertices), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), static_cast<void*>(&indices), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // texture attribute
    // NOLINTNEXTLINE(performance-no-int-to-ptr, cppcoreguidelines-pro-type-cstyle-cast)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return VAO;
}

auto render(Shader& shader_program, unsigned int VAO, unsigned int texture1, unsigned int texture2)
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

    shader_program.use();

    // create View matrix
    auto view = camera.get_view_matrix();

    // create Projection matrix
    auto projection = glm::perspective(glm::radians(camera.get_zoom()), 800.F / 600.F, 0.1F, 100.F);

    // Send matrices to shader (this is usually done each frame since transformation matrices tend
    // to change a lot)
    shader_program.set_mat4("view", view);
    shader_program.set_mat4("projection", projection);

    // Draw square
    glBindVertexArray(VAO);

    int i = 0;
    std::ranges::for_each(
      std::as_const(cube_positions), [&i, &shader_program](const auto& cube_position) {
          // create Model matrix
          auto model       = glm::mat4(1.F);
          model            = glm::translate(model, cube_position);
          const auto angle = 20.F * static_cast<float>(i++);
          model            = glm::rotate(model, glm::radians(angle), glm::vec3{1.F, 0.3F, 0.5F});
          shader_program.set_mat4("model", model);
          glDrawArrays(GL_TRIANGLES, 0, 36);
      });
}

auto create_texture(const char* path, bool is_PNG) -> unsigned int {
    auto texture = unsigned{0};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto width        = 0;
    auto height       = 0;
    auto num_channels = 0;
    auto* data        = stbi_load(path, &width, &height, &num_channels, 0);
    if (data != nullptr) {
        if (is_PNG) {
            glTexImage2D(
              GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        } else {
            glTexImage2D(
              GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        fmt::print("Failed to load texture\n");
    }
    stbi_image_free(static_cast<void*>(data));
    fmt::print("{} loaded\n", path);
    return texture;
}
}  // namespace

auto main() -> int {
    init_GLFW();
    auto* window = create_window();
    init_GLAD();
    set_callback_functions(window);

    auto shader_program = Shader{"./shaders/shader.vs", "./shaders/shader.fs"};
    auto VAO            = create_VAO();

    stbi_set_flip_vertically_on_load(1);
    const auto texture1 = create_texture("./resources/textures/container.jpg", false);
    const auto texture2 = create_texture("./resources/textures/grunge-scratch.png", true);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    shader_program.use();
    shader_program.set_int("texture1", 0);  // set it manually
    shader_program.set_int("texture2", 1);  // or with shader class

    glEnable(GL_DEPTH_TEST);

    auto delta_time = 0.F;  // Time between current frame and last frame
    auto last_frame = 0.F;  // Time of last frame

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