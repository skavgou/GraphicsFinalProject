#ifndef TILE_MANAGER_H
#define TILE_MANAGER_H

#include <unordered_map>
#include <tuple>
#include <vector>
#include <glm/glm.hpp>
#include "Building.h"

class TileManager {
private:
	std::unordered_map<std::tuple<int, int>, std::vector<Building>> activeTiles;
	glm::vec3 userPosition;
	const int TILE_SIZE = 100;
	const int GRID_RADIUS = 2;

	std::tuple<int, int> getCurrentGridPosition(const glm::vec3& position) const;
	std::vector<Building> generateTile(int gridX, int gridZ);

public:
	TileManager();
	void setUserPosition(const glm::vec3& position);
	const glm::vec3& getUserPosition() const;
	void updateTiles();
	void renderTiles(const glm::mat4& vpMatrix) const;
};

#endif // TILE_MANAGER_H
