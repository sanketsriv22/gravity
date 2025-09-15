#include <iostream>
#include <vector>

static const float gridSize = 10.0f;
static const float gridSpacing = 0.5f;

std::vector<float> generateGridVertices()
{
    std::vector<float> gridVertices;
    for (float i = -gridSize; i <= gridSize; i += gridSpacing)
    {
        // parallel to X axis
        gridVertices.push_back(-gridSize);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(i);

        gridVertices.push_back(gridSize);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(i);

        // parallel to Z axis
        gridVertices.push_back(i);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(-gridSize);

        gridVertices.push_back(i);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(gridSize);
    }

    return gridVertices;
}
