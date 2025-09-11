#pragma once
#include <iostream>
#include <vector>
#include "constants.h"
#include "structs.h"

class Body
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

    Body() : 
        radius{0.025f},
        numParts{50},
        mass{7.35e8f}, // how should i represent mass? scaled down moon mass for now
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

    void replacePosition(const Vector3& newPosition)
    {
        this->position = newPosition;
    }

    void replaceVelocity(const Vector3& newVelocity)
    {
        this->velocity = newVelocity;
    }

    void updatePosition(float deltaTime)
    {

        Vector3 scaledVelocity(this->velocity * deltaTime);
        Vector3 newPosition(this->position + scaledVelocity);
        replacePosition(newPosition);
        updateVertices();
    }

    void accelerate(float deltaTime)
    {
        Vector3 scaledAcceleration(this->acceleration * deltaTime);
        Vector3 newVelocity(this->velocity + scaledAcceleration);
        replaceVelocity(newVelocity);
    }

    void collisionCheck(float aspectRatio)
    {
        float limitX = aspectRatio;
        float limitY = 1.0f;
        // y check
        if (this->position.y - this->radius < -limitY)
        {
            this->position.y = -limitY + this->radius;
            updateVertices();
            this->velocity.y *= -0.95;
        }
        if (this->position.y + this->radius > limitY)
        {
            this->position.y = limitY - this->radius;
            updateVertices();
            this->velocity.y *= -0.95;
        }
        // x check
        if (this->position.x - this->radius < -limitX)
        {
            this->position.x = -limitX + this->radius;
            updateVertices();
            this->velocity.x *= -0.95;
        }
        if (this->position.x + this->radius > limitX)
        {
            this->position.x = limitX - this->radius;
            updateVertices();
            this->velocity.x *= -0.95;
        }
    }
};