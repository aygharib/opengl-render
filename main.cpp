#include <chrono>
#include <iostream>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    std::cout << "Hello world\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    return 0;
}