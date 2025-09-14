#include <iostream>
#include <vector>

#include "constants.h"
#include "structs.h"
#include "body.h"


struct System
{
    std::vector<Body*> planets;

    System(std::vector<Body*> bodies)
        : planets{bodies}
    {}

    void computeSystemProperties()
    {
        for (int i = 0; i < planets.size(); i++)
        {
            Vector3 totalAcceleration(0.0f, 0.0f, 0.0f);
            for (int j = 0; j < planets.size(); j++)
            {
                if (i==j) continue;

                Vector3 distance(planets[j]->position - planets[i]->position);

                distance.scalarMultiply(2.389e-10f);
                
                float r = sqrtf(pow(distance.x, 2) + pow(distance.y, 2) + pow(distance.z, 2) + 0.01f);
                
                // gravitational acceleration magnitude (mass of i reduces to 1)
                float a = (gravityConstant * planets[j]->mass) / pow(r, 2);

                Vector3 unitVector(distance.x/r, distance.y/r, distance.z/r);
                
                totalAcceleration += unitVector*a;

            }
            planets[i]->acceleration = totalAcceleration;
        }
    }
};