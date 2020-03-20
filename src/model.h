#pragma once

#include "mesh.h"
#include "shader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

class Model {
public:
  glm::vec3 pos;
  std::vector<Texture> textures_loaded;
  std::vector<Mesh> meshes;
  std::string directory;
  Model() {
  }
  Model(const char* path, glm::vec3 pos = glm::vec3(0.f, 0.f, 0.f)) : pos(pos) {
    loadModel(path);
  }
  void Draw(Shader shader);

private:
  void loadModel(std::string path);
  void processNode(aiNode* node, const aiScene* scene);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture>
    loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
