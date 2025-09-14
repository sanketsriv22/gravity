#pragma once

int SCREEN_WIDTH = 2400, SCREEN_HEIGHT = 1000;

float aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);

const float gravityEarth = 9.81f / 50000.0f; //not sure why i need to slow it down so much

// const float gravityConstant = 6.674e-11f; // true universal gravitational constant
const float gravityConstant = 6.674e-15f; // scaled universal gravitational constant

