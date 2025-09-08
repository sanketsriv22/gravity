#include <iostream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 800;
float aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);

const float gravityEarth = 9.81f / 50000.0f; //not sure why i need to slow it down so much

const float gravityConstant = 6.674e-11f; // universal gravitational constant

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

struct Color
{
    float R, G, B;
};

class Object
{
private:
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
public:
    float radius;
    const int numParts;
    float mass;
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    std::vector<float> vertices;
    Color color;

    Object() : 
        radius{0.025f},
        numParts{50},
        mass{7.35e9f}, // how should i represent mass? scaled down moon mass for now
        position{0.0f, 0.0f, 0.0f},
        velocity{0.0f, 0.0f, 0.0f},
        vertices{generateVertices()},
        acceleration{0.0f, 0.0f, 0.0f},
        color{0.0f, 0.0f, 1.0f}
        {};

    Vector3 const GetPosition()
    {
        return position;
    }

    void updateVertices()
    // simply updates the vertices vector
    {
        // update center
        this->vertices[0] = this->position.x;
        this->vertices[1] = this->position.y;
        this->vertices[2] = this->position.z;

        // update rest of vertices
        for (int i = 0; i <= this->numParts; i++)
        {
            float angle = 2.0f * 3.14159f * static_cast<float>(i)/this->numParts;
            // start with 3rd index, each vertex has 3 floats
            int idx = 3 + (i*3);
            this->vertices[idx]     = this->radius * cos(angle) + this->position.x;
            this->vertices[idx + 1] = this->radius * sin(angle) + this->position.y;
            this->vertices[idx + 2] = 0                         + this->position.z;   //unchanged for now
        }
    }

    void updatePosition(float deltaTime)
    {
        this->position.x += this->velocity.x * deltaTime;
        this->position.y += this->velocity.y * deltaTime;
        this->position.z += this->velocity.z * deltaTime;

        updateVertices();
    }

    void accelerate(float deltaTime)
    {
        this->velocity.x += this->acceleration.x * deltaTime;
        this->velocity.y += this->acceleration.y * deltaTime;
        this->velocity.z += this->acceleration.z * deltaTime;
    }

    void collisionCheck()
    {
        if (this->position.y - this->radius < -1.0f)
        {
            this->position.y = -1.0f + this->radius;
            updateVertices();
            this->velocity.y *= -0.95;
        }
        if (this->position.x + this->radius > 1.0f)
        {
            this->position.x = 1.0f - this->radius;
            updateVertices();
            this->velocity.x *= -0.95;
        }
        if (this->position.y + this->radius > 1.0f)
        {
            this->position.y = 1.0f - this->radius;
            updateVertices();
            this->velocity.y *= -0.95;
        }
        if (this->position.x - this->radius < -1.0f)
        {
            this->position.x = -1.0f + this->radius;
            updateVertices();
            this->velocity.x *= -0.95;
        }
    }
};


struct System
{
    std::vector<Object*> planets;

    System(std::vector<Object*> bodies)
        : planets{bodies}
    {}

    void computeSystemProperties()
    {
        for (int i = 0; i < planets.size(); i++)
        {
            Vector3 totalAcceleration = {0.0f, 0.0f, 0.0f};
            for (int j = 0; j < planets.size(); j++)
            {
                if (i==j) continue;
                Vector3 distance;
                distance.x = planets[j]->position.x - planets[i]->position.x;
                distance.y = planets[j]->position.y - planets[i]->position.y;
                distance.z = planets[j]->position.z - planets[i]->position.z;
                
                float r = sqrtf(pow(distance.x, 2) + pow(distance.y, 2) + pow(distance.z, 2) + 0.01f);
                
                // gravitational acceleration magnitude (mass of i reduces to 1)
                float a = (gravityConstant * planets[j]->mass) / pow(r, 2);

                Vector3 unitVector = {distance.x/r, distance.y/r, distance.z/r};

                totalAcceleration.x += a*unitVector.x;
                totalAcceleration.y += a*unitVector.y;
                totalAcceleration.z += a*unitVector.z;
            }
            planets[i]->acceleration = totalAcceleration;
        }
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
    GLint colorLocation = glGetUniformLocation(shaderProgram, "uColor");

    // create planets list
    std::vector<Object> planets;

    // add 2 planets
    planets.push_back(Object());
    planets.push_back(Object());
    planets.push_back(Object());
    planets.push_back(Object());

    float iV = 0.3e-1f;

    // offset starting values
    planets[0].color = {1.0f, 1.0f, 0.0f}; // yellow
    // planets[0].radius = 0.1f;
    // planets[0].mass = 5.97e10f;
    planets[0].position = {-0.3f, 0.0f, 0.0f};
    planets[0].velocity = {0.0f, iV, 0.0f};
    planets[0].updateVertices();

    planets[1].color = {0.0f, 1.0f, 1.0f}; // cyan
    planets[1].position = {0.3f, 0.0f, 0.0f};
    planets[1].velocity = {0.0f, -iV, 0.0f};
    planets[1].updateVertices();

    planets[2].color = {1.0f, 0.0f, 0.0f}; // red
    planets[2].position = {0.0f, 0.3f, 0.0f};
    planets[2].velocity = {iV, 0.0f, 0.0f};
    planets[2].updateVertices();
    
    planets[3].color = {1.0f, 0.0f, 1.0f}; // purp
    planets[3].position = {0.0f, -0.3f, 0.0f};
    planets[3].velocity = {iV, 0.0f, 0.0f};
    planets[3].updateVertices();

    // System system(planets[0], planets[1]);
    std::vector<Object*> planetPtrs;
    for (Object &obj: planets)
    {
        planetPtrs.push_back(&obj);
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

    while(!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // clear colors from each pixel every frame
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window); // created to exit on esc key press

        // render + draw
        aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
        glUniform1f(aspectRatioLocation, aspectRatio);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // update full system here
        system.computeSystemProperties();

        for (Object &planet : planets)
        {
            // update objects
            planet.collisionCheck();
            planet.accelerate(deltaTime);
            planet.updatePosition(deltaTime);

            // update vertex buffer for this planet
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, planet.vertices.size() * sizeof(float), planet.vertices.data());
            
            glUniform3f(colorLocation, planet.color.R, planet.color.G, planet.color.B);
            glDrawArrays(GL_TRIANGLE_FAN, 0, planet.vertices.size() / 3);
        }
        std::cout << planets[0].acceleration.x << std::endl;
        // std::cout << planets[0].acceleration.x << std::endl;
        // std::cout << planets[0].acceleration.z << std::endl;

        glfwSwapBuffers(window); // double buffer rendering
        glfwPollEvents();        // checks for any event triggering, updates window state and calls the right functions
    }

    glfwTerminate(); // end of glfwInit()
    return 0;
}