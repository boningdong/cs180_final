// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "light.h"

#define LIGHT_VERT_SHADER_PATH "shaders/forward_light.vs"
#define LIGHT_FRAG_SHADER_PATH "shaders/forward_light.fs"

// the model of the point light
// clang-format off
static float box_vertices[] = {
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
  0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
  0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
  0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
  0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
  0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
  0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

  -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 1.0f,

  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
  0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
  0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
  0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 1.0f,
  0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 1.0f,
  0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 1.0f,
  0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
  0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};
// clang-format on

// static variable definitions
unsigned int PointLight::vao = -1;
unsigned int PointLight::vbo = -1;
unsigned int PointLight::ebo = -1;
Shader* PointLight::shader = nullptr;

PointLight::PointLight(
  const glm::vec3 pos, const glm::vec3 color, float intensity, int dir, float speed) {
  this->pos = pos;
  this->color = color;
  this->intensity = intensity;
  this->dir = dir;
  this->speed = speed;
  if (shader == nullptr) setupLight();
}

void PointLight::draw(const glm::mat4& projection, const glm::mat4& view) {
  shader->use();
  shader->set_mat4("projection", projection);
  shader->set_mat4("view", view);
  shader->set_vec3("light_color", this->color);

  glm::mat4 model(1.0f);
  model = glm::translate(model, pos);
  model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
  shader->set_mat4("model", model);

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

void PointLight::setupLight() {
  // load shader
  shader = new Shader(LIGHT_VERT_SHADER_PATH, LIGHT_FRAG_SHADER_PATH);

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  // load
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(box_vertices), box_vertices, GL_STATIC_DRAW);

  // vertex positions
  // follows the layout of the shader - forward_light.vs
  unsigned int stride = sizeof(float) * 6;
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));

  glBindVertexArray(0);
}