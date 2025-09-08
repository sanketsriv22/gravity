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

    Object() : 
        radius{0.025f},
        numParts{50},
        mass{3.14159f*radius*radius*1e7f}, // how should i represent mass? area for now
        position{0.0f, 0.0f, 0.0f},
        velocity{0.0f, 0.0f, 0.0f},
        vertices{generateVertices()},
        acceleration{0.0f, 0.0f, 0.0f}
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

    void updatePosition()
    {
        this->position.x += this->velocity.x;
        this->position.y += this->velocity.y;
        this->position.z += this->velocity.z;

        updateVertices();
    }

    void accelerate()
    {
        this->velocity.x += this->acceleration.x;
        this->velocity.y += this->acceleration.y;
        this->velocity.z += this->acceleration.z;
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


struct twoBody
{
    std::vector<Object*> planets;
    Vector3 distances;
    float totalDistance;
    float force;

    // twoBody(std::vector<Object&> planets
    twoBody(Object& obj1, Object& obj2)
        : planets{&obj1, &obj2}, distances{0.0f, 0.0f, 0.0f},
          totalDistance(0.0f), force(0.0f)
    {}

    void componentDistanceCalculation()
    {
        this->distances.x = pow(planets[0]->position.x-planets[1]->position.x, 2.0f);
        this->distances.y = pow(planets[0]->position.y-planets[1]->position.y, 2.0f);
        this->distances.z = pow(planets[0]->position.z-planets[1]->position.z, 2.0f);
    }
    
    void totalDistanceCalculation()
    {
        this->totalDistance = sqrtf(this->distances.x + this->distances.y + this->distances.z);
    }


    void forceCalculation()
    {
        float numerator = gravityConstant * planets[1]->mass * planets[0]->mass;
        float denominator = pow(totalDistance, 2);
        force = numerator / denominator;
    }

    void calcSingleAccelerationComponents(Object &obj) 
    {
        float totalAcceleration = force / obj.mass;

        obj.acceleration.x = (distances.x / totalDistance) * totalAcceleration;
        obj.acceleration.y = (distances.y / totalDistance) * totalAcceleration;
        obj.acceleration.z = (distances.z / totalDistance) * totalAcceleration;
    }

    void calculateSystemProperties()
    {
        componentDistanceCalculation();
        totalDistanceCalculation();
        forceCalculation();
        for (Object *obj: planets)
        {
            calcSingleAccelerationComponents(*obj);
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

    // create planets list
    std::vector<Object> planets;

    // add 2 planets
    planets.push_back(Object());
    planets.push_back(Object());

    // offset starting position
    planets[0].position = {-0.2f, 0.0f, 0.0f};
    planets[0].updateVertices();
    planets[1].position = {+0.2f, 0.0f, 0.0f};
    planets[1].updateVertices();

    twoBody system(planets[0], planets[1]);

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

        // if deltaTime
        lastTime = currentTime;

        // clear colors from each pixel every frame
        glClear(GL_COLOR_BUFFER_BIT);

        processInput(window); // created to exit on esc key press

        // render + draw
        aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
        glUniform1f(aspectRatioLocation, aspectRatio);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // update twoBody here
        system.calculateSystemProperties();

        for (Object &planet : planets)
        {
            // update objects
            planet.collisionCheck();
            // based on other forces
            planet.accelerate();
            // based on current velocity
            planet.updatePosition();

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, planet.vertices.size() * sizeof(float), planet.vertices.data());
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