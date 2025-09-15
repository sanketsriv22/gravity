#pragma once

// planet shaders
const char *vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform float aspectRatio;\n"
    "out vec2 fragPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   vec3 correctedPos = aPos;\n"
    "   correctedPos.x /= aspectRatio;\n"
    // "   gl_Position = vec4(correctedPos, 1.0);\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   fragPos = aPos.xy;\n"
    "}\0";

const char *fragmentShaderSource = 
    "#version 330 core\n"
    "in vec2 fragPos;\n"               // input position from vertex shader
    "out vec4 FragColor;\n"
    "uniform vec2 planetCenter;\n"
    "uniform float planetRadius;\n"
    "uniform vec3 centerColor;\n"       // using center of planet, create gradient
    "uniform vec3 edgeColor;\n"
    // "uniform vec3 uColor;"
    "void main()\n"
    "{\n"
    // "   FragColor = vec4(1.0f, 0.1f, 0.2f, 1.0f);\n"
    "   float distance = length(fragPos - planetCenter);\n"
    "   float normalizedDistance = distance / planetRadius;\n"
    "   normalizedDistance = clamp(normalizedDistance, 0.0, 1.0);\n"
    "   vec3 color = mix(centerColor, edgeColor, normalizedDistance);\n"
    "   FragColor = vec4(color, 1.0f);\n"
    "}\0";


// spacetime grid shaders
const char *gridVertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * vec4(aPos, 1.0);\n"
    "}\0";


const char *gridFragmentShaderSource = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec3 uColor;"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(uColor, 1.0f);\n"
    "}\0";