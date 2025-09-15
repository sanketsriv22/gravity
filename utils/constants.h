#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int SCREEN_WIDTH = 2400, SCREEN_HEIGHT = 1000;

float aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);

const float gravityEarth = 9.81f / 50000.0f; //not sure why i need to slow it down so much

// const float gravityConstant = 6.674e-11f; // true universal gravitational constant
const float gravityConstant = 6.674e-15f; // scaled universal gravitational constant

// camera vars
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float camVConst = 0.2f;

float yaw = -90.0f; // horizontal angle
float pitch = 0.0f; // vertical angle

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

float fov = 45.0f;


