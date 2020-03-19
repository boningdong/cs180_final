#pragma once
#include "light.h"
#include "model.h"
#include "shader.h"

#include <vector>

class Scene {
public:
  std::vector<Model> objects;
  std::vector<PointLight> point_lights;
};