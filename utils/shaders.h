#pragma once

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform float aspectRatio;\n"
    "void main()\n"
    "{\n"
    "   vec3 correctedPos = aPos;\n"
    "   correctedPos.x /= aspectRatio;\n"
    "   gl_Position = vec4(correctedPos, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "uniform vec3 uColor;"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    // "   FragColor = vec4(1.0f, 0.1f, 0.2f, 1.0f);\n"
    "   FragColor = vec4(uColor, 1.0f);\n"
    "}\0";