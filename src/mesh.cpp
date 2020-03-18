#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include "stb_image.h"
#include "mesh.h"

// Mesh class
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;
  setupMesh();
}

void Mesh::setupMesh() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

  // release the bind
  glBindVertexArray(0);  
}

void Mesh::Draw(Shader shader) {
  unsigned int diffuseCount = 1;
  unsigned int specularCount = 1;
  unsigned int normalCount = 1;
  unsigned int heightCount = 1;
  for (unsigned int i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = textures[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(diffuseCount++);
    else if (name == "texture_specular")
      number = std::to_string(specularCount++);
    else if(name == "texture_normal")
      number = std::to_string(normalCount++); // transfer unsigned int to stream
    else if(name == "texture_height")
      number = std::to_string(heightCount++); // transfer unsigned int to stream
    shader.set_int(("material." + name + number).c_str(), i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
