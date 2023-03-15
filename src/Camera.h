#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from
// window-system specific input methods
enum class Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

// An abstract camera class that processes input and calculates the corresponding Euler Angles,
// Vectors and Matrices for use in OpenGL
class Camera {
   private:
    // camera attributes
    glm::vec3 up{0.F, 0.F, -1.F};
    glm::vec3 front{0.F, 0.F, -1.F};
    glm::vec3 right{0.F, 0.F, 0.F};
    glm::vec3 position{0.F, 0.F, 0.F};

    // world attributes
    glm::vec3 worldUp{0.F, 1.F, 0.F};

    // euler angles
    double yaw{-90.};
    double pitch{0.};

    // camera options
    float movement_speed{2.5F};
    float mouse_sensitivity{0.1F};
    float zoom{45.F};

   public:
    Camera(glm::vec3 position = glm::vec3(0.F, 0.F, 0.F));

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    auto get_view_matrix() -> glm::mat4;

    // processes input received from any keyboard-like input system. Accepts input parameter in the
    // form of camera defined ENUM (to abstract it from windowing systems)
    auto process_keyboard(Camera_Movement direction, float deltaTime) -> void;

    // processes input received from a mouse input system. Expects the offset value in both the x
    // and y direction.
    auto process_mouse_movement(double xoffset, double yoffset, GLboolean constrainPitch = 1)
      -> void;

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical
    // wheel-axis
    auto process_mouse_scroll(float yoffset) -> void;

    [[nodiscard]] auto get_zoom() const -> float;

   private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    auto update_camera_vectors() -> void;
};