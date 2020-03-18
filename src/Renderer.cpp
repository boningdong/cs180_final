#include "renderer.h"
#include "mesh.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdlib.h>

#define VERTEX_SHADER_PATH "shaders/vertex.vs"
#define FRAGMENT_SHADER_PATH "shaders/fragment.fs"
#define FRAGMETN_LIGHT_PATH "shaders/lightfrag.fs"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define MOUSE_SENS 0.01f
#define CAMERA_SPEED 0.005f

#define FOV_MAX 45.0f
#define FOV_MIN 1.0f
#define DEFAULT_FOV FOV_MAX

// Prevent flipping
#define PITCH_MAX 89.0f
#define PITCH_MIN -89.0f
#define DEFAULT_PITCH 0

#define DEFAULT_YAW -90

const static glm::vec3 WORLD_SPACE_UP(0, 1, 0);
const static glm::vec3 DEFAULT_CAMERA_POS(0, 0, 3);
const static glm::vec3 DEFAULT_CAMERA_DIR(0, 0, -1);
const static glm::vec3 LIGHT_COLOR(1.0f, 1.0f, 1.0f);
const static glm::vec3 OBJECT_COLOR(1.0f, 0.5f, 0.31f);

Renderer *Renderer::instance = nullptr;
Renderer *callback_handler = nullptr;

// non-static class members cannot be registered as callbacks, workaround with
// singleton
void resize_callback(GLFWwindow *window, int width, int height) {
  if (callback_handler)
    callback_handler->_resize(width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (callback_handler)
    callback_handler->_handle_mouse(xpos, ypos);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  if (callback_handler)
    callback_handler->_handle_scroll(yoffset);
}

// clang-format off
float vertices[] = {
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f,

  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 1.0f,
  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 1.0f,
  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 1.0f,
  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f
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
glm::vec3 light_position = glm::vec3(1.2f, 1.0f, 1.0f);
// clang-format on

Renderer *Renderer::get_instance() {
  if (!instance)
    instance = new Renderer;
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
  window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CS180 Final", NULL, NULL);
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
  // lightShader = new Shader(VERTEX_SHADER_PATH, FRAGMETN_LIGHT_PATH);

  // load model here
  char actual_path[PATH_MAX + 1];
  char *ptr = realpath("res/models/nanosuit/nanosuit.obj", actual_path);
  nanosuit = Model(actual_path);

  // register the callback functions
  glfwSetFramebufferSizeCallback(window, resize_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // capture mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // initialize viewing parameters
  camera_pos = DEFAULT_CAMERA_POS;
  camera_dir = DEFAULT_CAMERA_DIR;
  fov = DEFAULT_FOV;
  pitch = DEFAULT_PITCH;
  yaw = DEFAULT_YAW;
}

Renderer::~Renderer() {
  delete shader;
  // clean all of the GLFW's resources
  glfwTerminate();
}

void Renderer::loop() {
  while (!glfwWindowShouldClose(window)) {
    // process input
    handle_keyboard();

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

void Renderer::render() {
  glm::mat4 projection = glm::mat4(1.0f);
  projection = glm::perspective(glm::radians(fov),
                                (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
                                0.1f, 100.0f);
  set_projection(projection);

  glm::mat4 view = glm::mat4(1.0f);
  view = glm::lookAt(camera_pos, camera_pos + camera_dir, WORLD_SPACE_UP);
  set_view(view);

  // renders obbjects
  shader->use();
  shader->set_vec3("view_pos", camera_pos);
  shader->set_vec3("light_pos", camera_pos);
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
  model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
  shader->set_mat4("model", model);
  nanosuit.Draw(*shader);

  // draw contents of VBO (bound from VAO)
  // glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::_resize(int width, int height) {
  glViewport(0, 0, width, height);
}

void Renderer::_handle_mouse(int xpos, int ypos) {
  static int xprev = xpos;
  static int yprev = ypos;

  yaw += (xpos - xprev) * MOUSE_SENS;
  pitch += (yprev - ypos) * MOUSE_SENS; // invert y since 0 == top
  xprev = xpos;
  yprev = ypos;

  if (pitch < PITCH_MIN)
    pitch = PITCH_MIN;
  else if (pitch > PITCH_MAX)
    pitch = PITCH_MAX;

  // calculate direction from geometry
  camera_dir =
      normalize(glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                          sin(glm::radians(pitch)),
                          sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}

void Renderer::_handle_scroll(double offset) {
  if (fov >= FOV_MIN && fov <= FOV_MAX)
    fov -= offset;
  if (fov < FOV_MIN)
    fov = FOV_MIN;
  else if (fov > FOV_MAX)
    fov = FOV_MAX;
}

void Renderer::handle_keyboard(void) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera_pos += camera_dir * CAMERA_SPEED;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera_pos -= camera_dir * CAMERA_SPEED;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    // move in the direction orthogonal to the camera direction and up vector
    camera_pos +=
        glm::normalize(glm::cross(camera_dir, WORLD_SPACE_UP)) * CAMERA_SPEED;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    // move in the direction orthogonal to the camera direction and up vector
    camera_pos -=
        glm::normalize(glm::cross(camera_dir, WORLD_SPACE_UP)) * CAMERA_SPEED;
  }
}

void Renderer::set_model(glm::mat4 &model) { shader->set_mat4("model", model); }

void Renderer::set_view(glm::mat4 &view) { shader->set_mat4("view", view); }

void Renderer::set_projection(glm::mat4 &projection) {
  shader->set_mat4("projection", projection);
}

void Renderer::load_model() {
  glBindVertexArray(vao);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}
