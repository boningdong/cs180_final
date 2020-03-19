#pragma once
#include "model.h"
#include "shader.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#define LIGHT_VERT_SHADER_PATH "shaders/vertLight.vs"
#define LIGHT_FRAG_SHADER_PATH "shaders/fragLight.fs"

class PointLight {
public:
  glm::vec3 pos;
  glm::vec3 color;
  float intensity;
  PointLight(glm::vec3 pos, glm::vec3 color, float intensity);
  void draw(glm::mat4 projection, glm::mat4 view);

private:
  static unsigned int vao, vbo, ebo;
  static void setupLight();
  static Shader* shader;
};