#include "renderer.h"
#include "mesh.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdlib.h>
#include "light.h"

#define VERTEX_SHADER_PATH "shaders/vertModel.vs"
#define FRAGMENT_SHADER_PATH "shaders/fragModel.fs"
#define FRAGMETN_LIGHT_PATH "shaders/fragLight.fs"

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

  // initialize scene
  scene = Scene();
  // load model here
  char actual_path[PATH_MAX + 1];
  char *ptr = realpath("res/models/nanosuit/nanosuit.obj", actual_path);
  Model model = Model(actual_path, glm::vec3(0, 0, 0));
  
  // load models to the scene
  for (int i = 0; i < 4; i++) {
    model.pos = glm::vec3(0, 0, -1.0f * i);
    scene.objects.push_back(model);
  }

  // load lights to the scene
  for (int i = 0; i < 5; i++) {
    glm::vec3 pos(-2.0f, 2.0f, -1.0f * i);
    glm::vec3 color(0.25f * i, 1.0f, 1 - 0.25f * i);
    PointLight light = PointLight(pos, color, 1.0f); 
    scene.point_lights.push_back(light); 
  }

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

    // apply transformations and draw
    render();

    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Renderer::render() {
  shader->use();

  glm::mat4 projection = glm::mat4(1.0f);
  projection = glm::perspective(glm::radians(fov),
                                (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
                                0.1f, 100.0f);
  set_projection(projection);

  glm::mat4 view = glm::mat4(1.0f);
  view = glm::lookAt(camera_pos, camera_pos + camera_dir, WORLD_SPACE_UP);
  set_view(view);

  // renders obbjects
  shader->set_vec3("view_pos", camera_pos);
  shader->set_vec3("light_pos", camera_pos);

  // render all of the objects
  for(unsigned int i = 0; i < scene.objects.size(); i++) {
    Model object = scene.objects[i];
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, object.pos);
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
    shader->set_mat4("model", model);
    object.Draw(*shader);
  }
  
  // render all of the light source using forward shading
  // the shader is bound with the lightning class.
  for(unsigned int i = 0; i < scene.point_lights.size(); i++) {
    PointLight light = scene.point_lights[i];
    light.draw(projection, view);
  }
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
