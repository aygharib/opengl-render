#include "Shader.h"

#include <filesystem>
#include <fmt/core.h>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream;
        std::stringstream fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure e) {
        fmt::print("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n");
    }
    const auto* vShaderCode = vertexCode.c_str();
    const auto* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    auto vertex = 0;
    auto fragment = 0;
    auto success = 0;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        fmt::print("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n {} \n",
                   infoLog);
    }

    // similiar for Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        fmt::print("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n {} \n",
                   infoLog);
    }

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        fmt::print("ERROR::SHADER::PROGRAM::LINKING_FAILED\n {} \n", infoLog);
    }

    // delete the shaders as they're linked into our program now and no longer
    // necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

auto Shader::use() const -> void {
    glUseProgram(ID);
}

auto Shader::set_bool(const std::string& name, bool value) const -> void {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}

auto Shader::set_int(const std::string& name, int value) const -> void {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

auto Shader::set_float(const std::string& name, float value) const -> void {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

auto Shader::set_mat4(const std::string& name, const glm::mat4& mat) const
    -> void {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       &mat[0][0]);
}
