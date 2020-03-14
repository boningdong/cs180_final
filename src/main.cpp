
#include "renderer.h"

#include <iostream>

int main() {
    Renderer* renderer = Renderer::get_instance();
    renderer->load_texture("res/textures/container.jpg", 0);
    renderer->loop();

    return 0;
}