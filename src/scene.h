#pragma once
#include <vector>
#include "model.h"
#include "shader.h"
#include "light.h"

class Scene {
public:
  std::vector<Model> objects;
  std::vector<PointLight> point_lights;
};