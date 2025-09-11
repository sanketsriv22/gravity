#pragma once
#include <iostream>
#include <constants.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// helper methods and callbacks for GLFW

// resizing callback for GLFW
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    glViewport(0, 0, width, height);
}

// use for escaping program
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}