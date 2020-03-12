#pragma once

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
    // assign a global variable of type int in shader
    void set_vec4i(const std::string& name, int v0, int v1, int v2, int v3) const;
    // assign a global variable of type float in shader
    void set_vec4f(const std::string& name, float v0, float v1, float v2, float v3) const;

  private:
    shader_id_t shader_id = 0;
};