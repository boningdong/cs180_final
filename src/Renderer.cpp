#include "renderer.h"

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define VERTEX_SHADER_PATH "shaders/vertex.vs"
#define FRAGMENT_SHADER_PATH "shaders/fragment.fs"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

const static glm::vec3 WORLD_SPACE_UP(0, 1, 0);
const static glm::vec3 DEFAULT_CAMERA_POS(0, 0, 3);
const static glm::vec3 CAMERA_DIRECTION(0, 0, -1);
const float CAMERA_SPEED = 0.005f;

Renderer* Renderer::instance = nullptr;
Renderer* callback_handler = nullptr;

// non-static class members cannot be registered as callbacks, workaround with singleton
void resize_callback(GLFWwindow* window, int width, int height) {
    if (callback_handler) callback_handler->_resize(width, height);
}

// clang-format off
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
// world space positions of our cubes
glm::vec3 cube_positions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};
// clang-format on

Renderer* Renderer::get_instance() {
    if (!instance) instance = new Renderer;
    return instance;
}

Renderer::Renderer() {
    callback_handler = this;
    glfwInit();
    // make sure the opengl version is 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // generate window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CS180 Final", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        exit(1);
    }
    glfwMakeContextCurrent(window);

    // initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }

    // use Z-buffer
    glEnable(GL_DEPTH_TEST);

    // compile and initialize shaders
    shader = new Shader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

    // generate gl object for vertex array (attributes)
    glGenVertexArrays(1, &vao);
    // generate gl object for vertex buffer (data)
    glGenBuffers(1, &vbo);
    // generate gl object for element buffer (unique indices)
    // glGenBuffers(1, &ebo);

    // bind vertex array object (to save vertex configuration)
    glBindVertexArray(vao);
    // bind vertex buffer object (buffer for data)
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // copy data from vbo buffer to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // configure vertex attributes (will be saved to vao)
    // * location 0
    // * number of items in structure
    // * vertex structure uses floating point
    // * unnormalized (only needed for integer type)
    // * size of vertex structure in bytes
    // * offset in bytes into structure
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coords attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // register the callback functions
    glfwSetFramebufferSizeCallback(window, resize_callback);

    // initialize camera position
    camera_pos = DEFAULT_CAMERA_POS;
}

bool Renderer::load_texture(const char* path, int slot) {
    // generate a texture object
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, n_channels;
    unsigned char* data = stbi_load(path, &width, &height, &n_channels, 0);
    // generate the texture
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
        return false;
    }
    stbi_image_free(data);
    // load to TEXTURE{SLOT}
    shader->set_int("texture", slot);

    return true;
}

void Renderer::loop() {
    while (!glfwWindowShouldClose(window)) {
        // process input
        handle_input();

        // clear color buffer and depth buffer
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // load texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // load shaders
        shader->use();

        // apply transformations and draw
        render();

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Renderer::_resize(int width, int height) {
    glViewport(0, 0, width, height);
}

Renderer::~Renderer() {
    delete shader;
    // clean all of the GLFW's resources
    glfwTerminate();
}

void Renderer::render() {
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(
      glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    set_projection(projection);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(camera_pos, camera_pos + CAMERA_DIRECTION, WORLD_SPACE_UP);
    set_view(view);

    // load vertex attribute configuration
    glBindVertexArray(vao);

    for (unsigned int i = 0; i < 10; i++) {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cube_positions[i]);
        model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
        set_model(model);

        // draw contents of VBO (bound from VAO)
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void Renderer::handle_input(void) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_pos += CAMERA_SPEED * CAMERA_DIRECTION;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_pos -= CAMERA_SPEED * CAMERA_DIRECTION;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_pos += glm::normalize(glm::cross(CAMERA_DIRECTION, WORLD_SPACE_UP)) * CAMERA_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_pos -= glm::normalize(glm::cross(CAMERA_DIRECTION, WORLD_SPACE_UP)) * CAMERA_SPEED;
    }
}

void Renderer::set_model(glm::mat4& model) {
    shader->set_mat4("model", model);
}

void Renderer::set_view(glm::mat4& view) {
    shader->set_mat4("view", view);
}

void Renderer::set_projection(glm::mat4& projection) {
    shader->set_mat4("projection", projection);
}