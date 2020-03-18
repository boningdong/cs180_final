#pragma once
#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "mesh.h"

class Model {
public:
  std::vector<Texture> textures_loaded;
  std::vector<Mesh> meshes;
  std::string directory;
  Model() {}
  Model(const char* path) { loadModel(path); }
  void Draw(Shader shader);
  
private:
  void loadModel(std::string path);
  void processNode(aiNode *node, const aiScene* scene);
  Mesh processMesh(aiMesh *mesh, const aiScene* scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
