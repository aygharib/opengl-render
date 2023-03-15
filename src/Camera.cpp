#include "Camera.h"

Camera::Camera(glm::vec3 position) : position(position) { updateCameraVectors(); }

auto Camera::getViewMatrix() -> glm::mat4 { return glm::lookAt(position, position + front, up); }

auto Camera::processKeyboard(Camera_Movement direction, float deltaTime) -> void {
    float velocity = movementSpeed * deltaTime;
    if (direction == Camera_Movement::FORWARD) position += front * velocity;
    if (direction == Camera_Movement::BACKWARD) position -= front * velocity;
    if (direction == Camera_Movement::LEFT) position -= right * velocity;
    if (direction == Camera_Movement::RIGHT) position += right * velocity;
}

auto Camera::processMouseMovement(double xoffset, double yoffset, GLboolean constrainPitch)
  -> void {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch != 0U) {
        if (pitch > 89.0) pitch = 89.0;
        if (pitch < -89.0) pitch = -89.0;
    }

    // update front, right and up Vectors using the updated Euler angles
    updateCameraVectors();
}

auto Camera::processMouseScroll(float yoffset) -> void {
    zoom -= yoffset;
    if (zoom < 1.F) zoom = 1.F;
    if (zoom > 45.F) zoom = 45.F;
}

auto Camera::getZoom() const -> float { return zoom; }

auto Camera::updateCameraVectors() -> void {
    // calculate the new front vector
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front      = glm::normalize(newFront);
    // also re-calculate the right and up vector
    right = glm::normalize(glm::cross(
      front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more
                         // you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}