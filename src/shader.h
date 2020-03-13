#pragma once

#include <glm/glm.hpp>
#include <string>

typedef unsigned int shader_id_t;

class Shader {
  public:
    // initializes a shader from GLSL source files
    Shader(const char* vertex_path, const char* fragment_path);

    // set as active shadser
    void use(void);
    // retrieve shader glfw id
    shader_id_t get_id(void) const;
    // assign a global variable of type int in shader
    void set_int(const std::string& name, int value) const;
    // assign a global variable of type float in shader
    void set_float(const std::string& name, float value) const;
    // assign a global variable of type bool in shader
    void set_bool(const std::string& name, bool value) const;
    // assign a global vectors/matrices
    void set_vec2(const std::string& name, const glm::vec2& value) const;
    void set_vec2(const std::string& name, float x, float y) const;
    void set_vec3(const std::string& name, const glm::vec3& value) const;
    void set_vec3(const std::string& name, float x, float y, float z) const;
    void set_vec4(const std::string& name, const glm::vec4& value) const;
    void set_vec4(const std::string& name, float x, float y, float z, float w) const;
    void set_mat2(const std::string& name, const glm::mat2& mat) const;
    void set_mat3(const std::string& name, const glm::mat3& mat) const;
    void set_mat4(const std::string& name, const glm::mat4& mat) const;

  private:
    shader_id_t shader_id = 0;
};