#include <iostream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "constants.h"
#include "structs.h"
#include "body.h"
#include "system.h"
#include "shaders.h"
#include "helper_methods.h"

int main()
{
    std::cout << "Beginning an OpenGL project that simulates gravity." << std::endl;

    // init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // create a window
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gravity Simulation", NULL, NULL);
    if (window==NULL) // returns null if doesn't work
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // end of glfwInit()
        return -1;
    }

    // callbacks
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // create a callback for window resizing!
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // initialize GLAD?
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // enable 3D depth, adding depth buffer
    glEnable(GL_DEPTH_TEST);


    
    // compile a vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // compile fragment shader - calculate color output of pixels
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Final linked version of multiple shaders combined
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // get uniform location for aspect ratio
    GLint aspectRatioLocation = glGetUniformLocation(shaderProgram, "aspectRatio"); 
    GLint planetCenterLocation = glGetUniformLocation(shaderProgram, "planetCenter");
    GLint planetRadiusLocation = glGetUniformLocation(shaderProgram, "planetRadius");
    GLint centerColorLocation = glGetUniformLocation(shaderProgram, "centerColor");
    GLint edgeColorLocation = glGetUniformLocation(shaderProgram, "edgeColor");

    // for 3D perspective
    GLint modelLoc      = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc       = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    // projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.0001f, 100.0f);


    // FIGURE OUT A WAY TO CREATE SYSTEM MORE EFFICIENTLY
    // theres a lot of copy overhead here in construction of bodies

    // create planets list 
    std::vector<Body> planets;

    // add 4 planets
    planets.push_back(Body());
    // planets.push_back(Body());
    // planets.push_back(Body());
    // planets.push_back(Body());

    float iV = 9e-2f; // initial velocity

    // offset starting values
    planets[0].centerColor = {1.0f, 1.0f, 0.0f}; // yellow
    planets[0].radius = 0.05f;
    // planets[0].radius = 0.5f;
    planets[0].mass = 5.97e11f;
    planets[0].position = {0.0f, 0.0f, 0.0f};
    planets[0].velocity = {0.0f, 0.0f, 0.0f};
    planets[0].updateVertices();

    // planets[1].centerColor = {0.0f, 1.0f, 1.0f}; // cyan
    // planets[1].position = {-0.2f, 0.0f, 0.0f};
    // planets[1].velocity = {0.0f, -iV, 0.0f};
    // planets[1].updateVertices();

    // planets[2].color = {1.0f, 0.0f, 0.0f}; // red
    // planets[2].position = {0.0f, 0.3f, 0.0f};
    // planets[2].velocity = {iV, 0.0f, 0.0f};
    // planets[2].updateVertices();
    
    // planets[3].color = {1.0f, 0.0f, 1.0f}; // purp
    // planets[3].position = {0.0f, -0.3f, 0.0f};
    // planets[3].velocity = {iV, 0.0f, 0.0f};
    // planets[3].updateVertices();

    // System system(planets[0], planets[1]);
    std::vector<Body*> planetPtrs;
    for (Body& body: planets)
    {
        planetPtrs.push_back(&body);
    } 

    System system(planetPtrs);

    // vertex array and buffer objects (is it good practice to have a separate vbo/vao for each planet?)
    GLuint VAO, VBO;

    // create and bind VAO first
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // create and setup VBO
    glGenBuffers(1, &VBO); // passing reference so no copy made
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, planets[0].vertices.size() * sizeof(float), planets[0].vertices.data(), GL_DYNAMIC_DRAW);
    
    // configure vertex attributes (once)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float lastTime = glfwGetTime();

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


    while(!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // clear colors from each pixel every frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // bitwise or operator takes bit masks as args

        // process all key inputs
        processInput(window, deltaTime);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // render + draw
        aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
        glUniform1f(aspectRatioLocation, aspectRatio);

        glUseProgram(shaderProgram);
    
        glBindVertexArray(VAO);

        // update full system here
        system.computeSystemProperties();

        for (Body &planet : planets)
        {
            // update objects
            planet.collisionCheck(aspectRatio);
            planet.accelerate(deltaTime);
            planet.updatePosition(deltaTime);

            // update vertex buffer for this planet
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, planet.vertices.size() * sizeof(float), planet.vertices.data());
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(planet.position.x, planet.position.y, planet.position.z));
            model = glm::scale(model, glm::vec3(planet.radius));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glUniform2f(planetCenterLocation, planet.GetPosition().x, planet.GetPosition().y);
            glUniform1f(planetRadiusLocation, planet.GetRadius());
            glUniform3f(centerColorLocation, planet.centerColor.R, planet.centerColor.G, planet.centerColor.B);
            glUniform3f(edgeColorLocation, planet.edgeColor.R, planet.edgeColor.G, planet.edgeColor.B);
            
            glDrawArrays(GL_TRIANGLE_FAN, 0, planet.vertices.size() / 3);
        }

        glfwSwapBuffers(window); // double buffer rendering
        glfwPollEvents();        // checks for any event triggering, updates window state and calls the right functions
    }

    glfwTerminate(); // end of glfwInit()
    return 0;
}