
#include "renderer.h"

#include <iostream>

int main() {
  Renderer *renderer = Renderer::get_instance();
  renderer->loop();

  return 0;
}