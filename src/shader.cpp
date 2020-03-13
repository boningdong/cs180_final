#include "shader.h"

#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>

Shader::Shader(const char* vertex_path, const char* fragment_path) {
    std::string vertex_source;
    std::string fragment_source;
    std::ifstream vertex_file;
    std::ifstream fragment_file;

    vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // read source code from file into string
        vertex_file.open(vertex_path);
        fragment_file.open(fragment_path);
        std::stringstream vertex_stream, fragment_stream;
        vertex_stream << vertex_file.rdbuf();
        fragment_stream << fragment_file.rdbuf();
        vertex_file.close();
        fragment_file.close();
        vertex_source = vertex_stream.str();
        fragment_source = fragment_stream.str();
    } catch (std::ifstream::failure e) {
        std::cout << "Failed to read shader sources" << std::endl;
    }

    int success;
    char log[512];
    const char* vertex_source_cstr = vertex_source.c_str();
    const char* fragment_source_cstr = fragment_source.c_str();

    // compile our custom vertex shader
    int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source_cstr, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, sizeof(log), NULL, log);
        std::cout << "Failed to compile vertex shader:\n" << log << std::endl;
    }

    // compile our custom fragment shader
    int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source_cstr, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, sizeof(log), NULL, log);
        std::cout << "Failed to compile fragment shader:\n" << log << std::endl;
    }

    // link our custom shaders
    shader_id = glCreateProgram();
    glAttachShader(shader_id, vertex_shader);
    glAttachShader(shader_id, fragment_shader);
    glLinkProgram(shader_id);
    glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader_id, sizeof(log), NULL, log);
        std::cout << "Failed to link shaders:\n" << log << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::use() {
    glUseProgram(shader_id);
}

shader_id_t Shader::get_id(void) const {
    return shader_id;
}

void Shader::set_int(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(shader_id, name.c_str()), value);
}

void Shader::set_float(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(shader_id, name.c_str()), value);
}

void Shader::set_bool(const std::string& name, bool value) const {
    set_int(name, (int)value);
}

void Shader::set_vec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]);
}

void Shader::set_vec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(shader_id, name.c_str()), x, y);
}

void Shader::set_vec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]);
}

void Shader::set_vec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(shader_id, name.c_str()), x, y, z);
}

void Shader::set_vec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(shader_id, name.c_str()), 1, &value[0]);
}

void Shader::set_vec4(const std::string& name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(shader_id, name.c_str()), x, y, z, w);
}

void Shader::set_mat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(glGetUniformLocation(shader_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(shader_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(shader_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}