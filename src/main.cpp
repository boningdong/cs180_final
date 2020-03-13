
#include "renderer.h"

#include <iostream>

int main() {
    Renderer renderer("CS 180 Project", 800, 600);
    renderer.load_texture("res/textures/container.jpg", 0);
    renderer.loop();

    return 0;
}