#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shader.h"
#include <assimp/scene.h>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

struct Texture {
  unsigned int id;
  std::string type;
  std::string path; // need this because we want to compare strings to see if we are trying to load the same texture.
};

class Mesh {
public:
  unsigned int vao;
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
  void Draw(Shader shader);
private:
  unsigned int vbo, ebo;
  void setupMesh();
};


