#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
private:
    // camera attributes
    glm::vec3 up{0.F, 0.F, -1.F};
    glm::vec3 front{0.F, 0.F, -1.F};
    glm::vec3 right{0.F, 0.F, 0.F};
    glm::vec3 position;

    // world attributes
    glm::vec3 worldUp{0.F, 1.F, 0.F};

    // euler angles
    double yaw{-90.};
    double pitch{0.};
    
    // camera options
    float movementSpeed{2.5F};
    float mouseSensitivity{0.1F};
    float zoom{45.F};

public:
    Camera(glm::vec3 position = glm::vec3(0.F, 0.F, 0.F)) : position(position) {
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    auto getViewMatrix() -> glm::mat4 {
        return glm::lookAt(position, position + front, up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    auto processKeyboard(Camera_Movement direction, float deltaTime) -> void {
        float velocity = movementSpeed * deltaTime;
        if (direction == FORWARD)
            position += front * velocity;
        if (direction == BACKWARD)
            position -= front * velocity;
        if (direction == LEFT)
            position -= right * velocity;
        if (direction == RIGHT)
            position += right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    auto processMouseMovement(double xoffset, double yoffset, GLboolean constrainPitch = 1) -> void {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw   += xoffset;
        pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch != 0U) {
            if (pitch > 89.0)
                pitch = 89.0;
            if (pitch < -89.0)
                pitch = -89.0;
        }

        // update front, right and up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    auto processMouseScroll(float yoffset) -> void {
        zoom -= yoffset;
        if (zoom < 1.F)
            zoom = 1.F;
        if (zoom > 45.F)
            zoom = 45.F;
    }

    [[nodiscard]] auto getZoom() const -> float {
        return zoom;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    auto updateCameraVectors() -> void {
        // calculate the new front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);
        // also re-calculate the right and up vector
        right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up    = glm::normalize(glm::cross(right, front));
    }
};
#endif