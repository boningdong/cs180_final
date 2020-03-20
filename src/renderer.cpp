#include "renderer.h"

#include "light.h"
#include "mesh.h"
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#define realpath(N, R) _fullpath((R), (N), _MAX_PATH)
#endif

#define USE_DEFERRED_SHADING

#define NR_LIGHTS 50 // also update in deferred_light.fs

#define FORWARD_VERTEX_SHADER_PATH "shaders/forward_model.vs"
#define FORWARD_FRAGMENT_SHADER_PATH "shaders/forward_model.fs"
#define DEFERRED_GEOMETRY_VERTEX_SHADER_PATH "shaders/deferred_geometry.vs"
#define DEFERRED_GEOMETRY_FRAGMENT_SHADER_PATH "shaders/deferred_geometry.fs"
#define DEFERRED_LIGHT_VERTEX_SHADER_PATH "shaders/deferred_light.vs"
#define DEFERRED_LIGHT_FRAGMENT_SHADER_PATH "shaders/deferred_light.fs"

// desired window size
#define _WINDOW_WIDTH 1280
#define _WINDOW_HEIGHT 720

// will be determined by framebuffer - platform specific
int WINDOW_WIDTH;
int WINDOW_HEIGHT;

#define MOUSE_SENS 0.05f
#define CAMERA_SPEED 2.5f

#define FOV_MAX 45.0f
#define FOV_MIN 1.0f
#define DEFAULT_FOV FOV_MAX

// prevent flipping
#define PITCH_MAX 89.0f
#define PITCH_MIN -89.0f
#define DEFAULT_PITCH 0
#define DEFAULT_YAW -90

#define RAND_DIST(min, max) (min + ((float)rand() / (float)RAND_MAX) * (max - min))

const static glm::vec3 WORLD_SPACE_UP(0, 1, 0);
const static glm::vec3 DEFAULT_CAMERA_POS(0, 1, 3);
const static glm::vec3 DEFAULT_CAMERA_DIR(0, 0, -1);
const static glm::vec3 LIGHT_COLOR(1.0f, 1.0f, 1.0f);
const static glm::vec3 OBJECT_COLOR(1.0f, 0.5f, 0.31f);

static float quad_vertices[] = {
  -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
  1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
};

Renderer* Renderer::instance = nullptr;
Renderer* callback_handler = nullptr;

// non-static class members cannot be registered as callbacks, workaround with
// singleton
void resize_callback(GLFWwindow* window, int width, int height) {
  if (callback_handler) callback_handler->_resize(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  if (callback_handler) callback_handler->_handle_mouse(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  if (callback_handler) callback_handler->_handle_scroll(yoffset);
}

Renderer* Renderer::get_instance() {
  if (!instance) instance = new Renderer;
  return instance;
}

Renderer::Renderer() {
  callback_handler = this;
  srand(time(NULL));
  glfwInit();
  // make sure the opengl version is 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // generate window
  window = glfwCreateWindow(_WINDOW_WIDTH, _WINDOW_HEIGHT, "CS180 Final", NULL, NULL);
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

  int frameBufferWidth, frameBufferHeight;
  glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
  WINDOW_HEIGHT = frameBufferHeight;
  WINDOW_WIDTH = frameBufferWidth;

  // use Z-buffer
  glEnable(GL_DEPTH_TEST);

  // compile and initialize shaders
  forward_shader = new Shader(FORWARD_VERTEX_SHADER_PATH, FORWARD_FRAGMENT_SHADER_PATH);

#ifdef USE_DEFERRED_SHADING
  init_deferred_engine();
#endif // USE_DEFERRED_SHADING

  // initialize scene
  scene = Scene();
  // load model here
  char actual_path[PATH_MAX + 1];
  char* ptr = realpath("res/models/nanosuit/nanosuit.obj", actual_path);
  Model model = Model(actual_path, glm::vec3(0, 0, 0));

  // load models to the scene
  for (int i = 0; i < 4; i++) {
    model.pos = glm::vec3(0, 0, -1.0f * i);
    scene.objects.push_back(model);
  }

  // load lights to the scene
  for (int i = 0; i < NR_LIGHTS; i++) {
    glm::vec3 pos(RAND_DIST(-5, 5), RAND_DIST(-5, 5), RAND_DIST(-5, 5));
    glm::vec3 color(RAND_DIST(0, 1), RAND_DIST(0, 1), RAND_DIST(0, 1));
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

void Renderer::init_deferred_engine() {
  deferred_geometry_shader =
    new Shader(DEFERRED_GEOMETRY_VERTEX_SHADER_PATH, DEFERRED_GEOMETRY_FRAGMENT_SHADER_PATH);
  deferred_light_shader =
    new Shader(DEFERRED_LIGHT_VERTEX_SHADER_PATH, DEFERRED_LIGHT_FRAGMENT_SHADER_PATH);

  glGenFramebuffers(1, &gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  // add attachments to the g-buffer
  // - position color buffer
  glGenTextures(1, &gPosition);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
  // - normal color buffer
  glGenTextures(1, &gNormal);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
  // - color + specular buffer
  glGenTextures(1, &gColorSpec);
  glBindTexture(GL_TEXTURE_2D, gColorSpec);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

  // tell opengl which color buffers to draw into
  unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0,
                                  GL_COLOR_ATTACHMENT1,
                                  GL_COLOR_ATTACHMENT2 };
  glDrawBuffers(3, attachments);

  glGenRenderbuffers(1, &rbo_depth);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
  // check if framebuffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Incomplete framebuffer" << std::endl;
  }
  // release the g-buffer after initialization
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // map texture IDs
  deferred_light_shader->use();
  deferred_light_shader->set_int("gPosition", 0);
  deferred_light_shader->set_int("gNormal", 1);
  deferred_light_shader->set_int("gColorSpec", 2);
}

Renderer::~Renderer() {
  delete forward_shader;
  delete deferred_geometry_shader;
  delete deferred_light_shader;
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
  glm::mat4 projection = glm::mat4(1.0f);
  projection =
    glm::perspective(glm::radians(fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

  glm::mat4 view = glm::mat4(1.0f);
  view = glm::lookAt(camera_pos, camera_pos + camera_dir, WORLD_SPACE_UP);

#ifdef USE_DEFERRED_SHADING
  // perform deferred rendering
  render_geometry(projection, view);
  render_lighting();
  render_quad();

  // copy depth information from gbuffer to default framebuffer
  glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(
    0,
    0,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    0,
    0,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    GL_DEPTH_BUFFER_BIT,
    GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else  // forward shading
  forward_shader->use();
  // set uniforms
  forward_shader->set_mat4("projection", projection);
  forward_shader->set_mat4("view", view);
  forward_shader->set_vec3("view_pos", camera_pos);
  forward_shader->set_vec3("light_pos", camera_pos);

  // render all of the objects
  for (unsigned int i = 0; i < scene.objects.size(); i++) {
    Model object = scene.objects[i];
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, object.pos);
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
    forward_shader->set_mat4("model", model);
    object.Draw(*forward_shader);
  }
#endif // USE_DEFERRED_SHADING

  // render all of the light source using forward shading
  // the shader is bound with the lighting class.
  for (unsigned int i = 0; i < scene.point_lights.size(); i++) {
    scene.point_lights[i].draw(projection, view);
  }
}

void Renderer::render_geometry(const glm::mat4& projection, const glm::mat4& view) {
  // geometry pass
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  deferred_geometry_shader->use();
  deferred_geometry_shader->set_mat4("projection", projection);
  deferred_geometry_shader->set_mat4("view", view);
  for (unsigned int i = 0; i < scene.objects.size(); i++) {
    Model* object = &scene.objects[i];
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, object->pos);
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
    deferred_geometry_shader->set_mat4("model", model);
    object->Draw(*deferred_geometry_shader);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::render_lighting() {
  // lighting pass
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  deferred_light_shader->use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gColorSpec);

  for (unsigned int i = 0; i < scene.point_lights.size(); i++) {
    // set light uniforms
    deferred_light_shader->set_vec3(
      "lights[" + std::to_string(i) + "].position", scene.point_lights[i].pos);
    deferred_light_shader->set_vec3(
      "lights[" + std::to_string(i) + "].color", scene.point_lights[i].color);
  }
  deferred_light_shader->set_vec3("view_pos", camera_pos);
}

void Renderer::render_quad() {
  if (vao == 0) {
    // setup plane VAO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    // vec3 pos, vec2 texture coord
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  }
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void Renderer::_resize(int width, int height) {
  // glViewport(0, 0, width, height);
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
  camera_dir = normalize(glm::vec3(
    cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
    sin(glm::radians(pitch)),
    sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}

void Renderer::_handle_scroll(double offset) {
  if (fov >= FOV_MIN && fov <= FOV_MAX) fov -= offset;
  if (fov < FOV_MIN)
    fov = FOV_MIN;
  else if (fov > FOV_MAX)
    fov = FOV_MAX;
}

void Renderer::handle_keyboard(void) {
  float t = glfwGetTime();
  dt = t - t_prev;
  t_prev = t;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera_pos += camera_dir * CAMERA_SPEED * dt;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera_pos -= camera_dir * CAMERA_SPEED * dt;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    // move in the direction orthogonal to the camera direction and up vector
    camera_pos += glm::normalize(glm::cross(camera_dir, WORLD_SPACE_UP)) * CAMERA_SPEED * dt;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    // move in the direction orthogonal to the camera direction and up vector
    camera_pos -= glm::normalize(glm::cross(camera_dir, WORLD_SPACE_UP)) * CAMERA_SPEED * dt;
  }
}