#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "Building.h"

#include <unordered_map>
#include <tuple>

// Tile size and active grid dimensions
const int TILE_SIZE = 100; // Size of one tile
const int GRID_RADIUS = 2; // Number of tiles in each direction (total 5x5)

// User position and tile tracking
glm::vec3 userPosition(0.0f);
std::unordered_map<std::tuple<int, int>, std::vector<Building>> activeTiles;

// Helper function to get the current grid position of the user
std::tuple<int, int> getCurrentGridPosition(const glm::vec3 &position) {
    int x = static_cast<int>(std::floor(position.x / TILE_SIZE));
    int z = static_cast<int>(std::floor(position.z / TILE_SIZE));
    return {x, z};
}


// A single tile containing multiple buildings
struct Tile {
    std::vector<Building> buildings;

    // Initialize the tile with a specific layout configuration
    void initialize(glm::vec3 origin, glm::vec3 tileSize, int layoutType) {
        buildings.clear();

        switch (layoutType) {
            case 1:
                // Simple grid layout
                createGridLayout(origin, tileSize);
                break;
            case 2:
                // Central plaza layout
                createPlazaLayout(origin, tileSize);
                break;
            case 3:
                // Randomized layout
                createRandomLayout(origin, tileSize);
                break;
            default:
                std::cerr << "Unknown layout type: " << layoutType << std::endl;
        }
    }

    // Render all buildings in the tile
    void render(glm::mat4 vp) {
        for (Building& building : buildings) {
            building.render(vp);
        }
    }

    // Clean up OpenGL resources for the buildings
    void cleanup() {
        for (Building& building : buildings) {
            building.cleanup();
        }
    }

private:
    // Create a grid layout of buildings
    void createGridLayout(glm::vec3 origin, glm::vec3 tileSize) {
        int rows = 4, cols = 4;
        glm::vec3 cellSize = glm::vec3(tileSize.x / cols, 1.0f, tileSize.z / rows);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                glm::vec3 position = origin + glm::vec3(j * cellSize.x, 0, i * cellSize.z);
                glm::vec3 scale = glm::vec3(cellSize.x * 0.8f, rand() % 50 + 10, cellSize.z * 0.8f);
                Building b;
                b.initialize(position, scale, rand() % 6 + 1);
                buildings.push_back(b);
            }
        }
    }

    // Create a central plaza layout with surrounding buildings
    void createPlazaLayout(glm::vec3 origin, glm::vec3 tileSize) {
        glm::vec3 center = origin + glm::vec3(tileSize.x / 2, 0, tileSize.z / 2);
        glm::vec3 plazaSize = glm::vec3(tileSize.x * 0.4f, 1.0f, tileSize.z * 0.4f);

        // Surrounding buildings
        for (int i = 0; i < 8; i++) {
            float angle = i * glm::radians(45.0f);
            glm::vec3 offset = glm::vec3(cos(angle), 0, sin(angle)) * tileSize.x * 0.3f;
            glm::vec3 position = center + offset;
            glm::vec3 scale = glm::vec3(tileSize.x * 0.2f, rand() % 50 + 20, tileSize.z * 0.2f);
            Building b;
            b.initialize(position, scale, rand() % 6 + 1);
            buildings.push_back(b);
        }
    }

    // Create a randomized layout of buildings
    void createRandomLayout(glm::vec3 origin, glm::vec3 tileSize) {
        int buildingCount = 10;

        for (int i = 0; i < buildingCount; i++) {
            glm::vec3 position = origin + glm::vec3(rand() % (int)tileSize.x, 0, rand() % (int)tileSize.z);
            glm::vec3 scale = glm::vec3(rand() % 50 + 10, rand() % 100 + 20, rand() % 50 + 10);
            Building b;
            b.initialize(position, scale, rand() % 6 + 1);
            buildings.push_back(b);
        }
    }
};