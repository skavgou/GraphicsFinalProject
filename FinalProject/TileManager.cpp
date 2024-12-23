#include "TileManager.h"
#include <glm/glm.hpp>
#include <cmath>
#include <cstdlib>

TileManager::TileManager() : userPosition(0.0f) {}

void TileManager::setUserPosition(const glm::vec3& position) {
    userPosition = position;
}

const glm::vec3& TileManager::getUserPosition() const {
    return userPosition;
}

std::tuple<int, int> TileManager::getCurrentGridPosition(const glm::vec3& position) const {
    int gridX = static_cast<int>(std::floor(position.x / TILE_SIZE));
    int gridZ = static_cast<int>(std::floor(position.z / TILE_SIZE));
    return {gridX, gridZ};
}

std::vector<Building> TileManager::generateTile(int gridX, int gridZ) {
    std::vector<Building> buildings;
    int seed = gridX * 73856093 ^ gridZ * 19349663;
    srand(seed);

    int numBuildings = rand() % 10 + 5;
    for (int i = 0; i < numBuildings; ++i) {
        Building b;
        float x = gridX * TILE_SIZE + rand() % TILE_SIZE;
        float z = gridZ * TILE_SIZE + rand() % TILE_SIZE;
        float height = rand() % 50 + 10;

        b.initialize(glm::vec3(x, height / 2, z), glm::vec3(10, height, 10), rand() % 6 + 1);
        buildings.push_back(b);
    }
    return buildings;
}

void TileManager::updateTiles() {
    auto [gridX, gridZ] = getCurrentGridPosition(userPosition);

    std::unordered_map<std::tuple<int, int>, std::vector<Building>> newActiveTiles;
    for (int dx = -GRID_RADIUS; dx <= GRID_RADIUS; ++dx) {
        for (int dz = -GRID_RADIUS; dz <= GRID_RADIUS; ++dz) {
            std::tuple<int, int> tileKey = {gridX + dx, gridZ + dz};
            if (activeTiles.find(tileKey) != activeTiles.end()) {
                newActiveTiles[tileKey] = activeTiles[tileKey];
            } else {
                newActiveTiles[tileKey] = generateTile(gridX + dx, gridZ + dz);
            }
        }
    }

    for (auto& [key, buildings] : activeTiles) {
        if (newActiveTiles.find(key) == newActiveTiles.end()) {
            for (Building& b : buildings) {
                b.cleanup();
            }
        }
    }

    activeTiles = std::move(newActiveTiles);
}

void TileManager::renderTiles(const glm::mat4& vpMatrix) const {
    for (const auto& [key, buildings] : activeTiles) {
        for (const Building& b : buildings) {
            b.render(vpMatrix);
        }
    }
}
