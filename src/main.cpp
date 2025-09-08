#include <iostream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 800;
float aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);

const float gravity = 9.81f / 10000.0f; //not sure why i need to slow it down so much

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
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.1f, 0.2f, 1.0f);\n"
    "}\0";

// resizing callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
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


struct Vector3
{
    float x, y, z;
};

class Ball
{
public:
    Vector3 position;
    Vector3 velocity;
    const float radius;
    const int numParts;
    std::vector<float> vertices;

    Ball() : 
        position{0.0f, 0.0f, 0.0f},
        radius{0.1f},
        numParts{50},
        vertices{generateVertices()},
        velocity{0.0f, 0.0f, 0.0f}
        {};

    Vector3 const GetPosition()
    {
        return position;
    }

    std::vector<float> generateVertices()
    {
        std::vector<float> vertices;
        vertices.reserve((numParts + 2) * 3);

        // center vertex
        vertices.insert(vertices.end(), {position.x, position.y, position.z});

        for (int i = 0; i<=numParts; i++)
        {
            float angle = 2.0f * 3.14159f * static_cast<float>(i)/numParts;
            vertices.insert(vertices.end(),
            {
                radius * cos(angle), // x    
                radius * sin(angle), // y
                position.z           // z
            });
        }
        return vertices;
    }

    void updateVertices()
    // simply updates the vertices vector
    {
        // update center
        vertices[0] = position.x;
        vertices[1] = position.y;
        vertices[2] = position.z;

        // update rest of vertices
        for (int i = 0; i <= numParts; i++)
        {
            float angle = 2.0f * 3.14159f * static_cast<float>(i)/numParts;
            // start with 3rd index, each vertex has 3 floats
            int idx = 3 + (i*3);
            vertices[idx]     = radius * cos(angle) + position.x;
            vertices[idx + 1] = radius * sin(angle) + position.y;
            vertices[idx + 2] = 0                   + position.z;   //unchanged for now
        }
    }

    void updatePosition(float deltaTime)
    {
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        position.z += velocity.z * deltaTime;
    }

    void updateVelocity(float deltaTime)
    {
        velocity.y +=  - gravity * deltaTime;
    }
};

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

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // create a callback for window resizing!

    // initialize GLAD?
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
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

    // get vertices for a circle
    Ball ball;
    ball.generateVertices();
    
    std::cout << "Generating circle with " << ball.vertices.size() / 3 << " vertices" << std::endl;

    // vertex array and buffer objects
    GLuint VAO, VBO;

    // create and bind VAO first
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // create and setup VBO
    glGenBuffers(1, &VBO); // passing reference so no copy made
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, ball.vertices.size() * sizeof(float), ball.vertices.data(), GL_DYNAMIC_DRAW);
    
    // configure vertex attributes (once)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float lastTime = glfwGetTime();

    while(!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // clear colors from each pixel every frame
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window); // created to exit on esc key press

        // based on gravity
        ball.updateVelocity(lastTime);

        // based on current velocity
        ball.updatePosition(lastTime);
        ball.updateVertices();

        if (ball.position.y - ball.radius < -1.0f)
        {
            // std::cout << "BOUNCED" << std::endl;
            ball.position.y = -1.0f + ball.radius;
            ball.updateVertices();
            ball.velocity.y *= -0.75;
        }

        // render + draw
        aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
        glUniform1f(aspectRatioLocation, aspectRatio);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, ball.vertices.size() * sizeof(float), ball.vertices.data());
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, ball.vertices.size() / 3);

        glfwSwapBuffers(window); // double buffer rendering
        glfwPollEvents();        // checks for any event triggering, updates window state and calls the right functions
    }

    glfwTerminate(); // end of glfwInit()
    return 0;
}