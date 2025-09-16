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
    std::vector<float> generateSphereVertices()
    {
        std::vector<float> vertices;
        vertices.reserve((latSegs + 1) * (longSegs + 1) * 3);

        for (int lat = 0; lat <= latSegs; lat++)
        {
            // latidude angle from pole to pole (0 to pi)
            float theta = lat * PI / latSegs;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (int longi = 0; longi <= longSegs; longi++)
            {
                // longitude angle from pole to pole (0 to 2pi)
                float phi = longi * 2.0f * PI / longSegs;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);
                
                vertices.insert(vertices.end(),
                {
                    radius * sinTheta * cosPhi + position.x,
                    radius * cosTheta + position.y,
                    radius * sinTheta * sinPhi + position.z
                });
            }
        }
        
        return vertices;
    }

    // for nonduplication of vertex renderings
    // prevent overlapping vertices from rendering more than once
    std::vector<unsigned int> generateSphereIndices()
    {
        std::vector<unsigned int> indices;
        indices.reserve(latSegs * longSegs * 6);

        for (int lat = 0; lat < latSegs; lat++)
        {
            for (int longi = 0; longi < longSegs; longi++)
            {
                // current vertex indices
                unsigned int current = lat * (longSegs + 1) + longi;
                unsigned int next = current + longSegs + 1;

                // first triangle
                indices.insert(indices.end(), {current, next, current + 1});
                // second triangle
                indices.insert(indices.end(), {current + 1, next, next + 1});
            }
        }

        return indices;
    }
public:
    float radius;
    const int numParts; // num triangles for 2D circle object
    const int latSegs;   // num of lat slices
    const int longSegs;  //num of long slices
    float mass;
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    Color centerColor;
    Color edgeColor;

    Body() : 
        radius{0.01f},
        numParts{50},
        latSegs{20},
        longSegs{40},
        mass{7.35e7f}, // how should i represent mass? scaled down moon mass for now
        position{0.0f, 0.0f, 0.0f},
        velocity{0.0f, 0.0f, 0.0f},
        vertices{generateSphereVertices()},
        indices{generateSphereIndices()},
        acceleration{0.0f, 0.0f, 0.0f},
        centerColor{0.0f, 0.0f, 1.0f},
        edgeColor{0.0f, 0.0f, 0.0f,}
        {};

    Vector3 const GetPosition() { return position; }
    float const GetRadius() { return radius; }

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
            float angle = 2.0f * PI * static_cast<float>(i)/this->numParts;
            // start with 3rd index, each vertex has 3 floats
            int idx = 3 + (i*3);
            this->vertices[idx]     = this->radius * cos(angle) + this->position.x;
            this->vertices[idx + 1] = this->radius * sin(angle) + this->position.y;
            this->vertices[idx + 2] = 0                            + this->position.z;   //unchanged for now
        }
    }

    void updateSphereVertices()
    {
        int vertexIndex = 0;
        for (int lat = 0; lat <= latSegs; lat++)
        {
            float theta = lat * PI / latSegs;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (int longi = 0; longi <= longSegs; longi++)
            {
                float phi = longi * 2.0f * PI / longSegs;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                // update vertex positions
                this->vertices[vertexIndex + 0] = radius * sinTheta * cosPhi + position.x;
                this->vertices[vertexIndex + 1] = radius * cosTheta + position.y;
                this->vertices[vertexIndex + 2] = radius * sinTheta * sinPhi + position.z;

                vertexIndex += 3;
            }
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

    void updatePosition(float& deltaTime)
    {

        Vector3 scaledVelocity(this->velocity * deltaTime);
        Vector3 newPosition(this->position + scaledVelocity);
        replacePosition(newPosition);
        updateSphereVertices();
    }

    void accelerate(float& deltaTime)
    {
        Vector3 scaledAcceleration(this->acceleration * deltaTime);
        Vector3 newVelocity(this->velocity + scaledAcceleration);
        replaceVelocity(newVelocity);
    }

    void collisionCheck(float& aspectRatio)
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