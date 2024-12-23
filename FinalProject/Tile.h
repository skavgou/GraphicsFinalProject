#ifndef TILE_H
#define TILE_H

#include <vector>
#include <unordered_map>
#include <tuple>
#include <glm/glm.hpp>
#include "Building.h"

// Hash specialization for std::tuple<int, int>
namespace std {
	template <>
	struct hash<std::tuple<int, int>> {
		std::size_t operator()(const std::tuple<int, int>& key) const noexcept {
			auto [x, y] = key;
			return std::hash<int>{}(x) ^ (std::hash<int>{}(y) << 1); // XOR and bit-shift combination
		}
	};
}

// Constants for tile dimensions and active grid
const int TILE_SIZE = 100;
const int GRID_RADIUS = 2;

// Functions for tile management
std::vector<Building> generateTile(int gridX, int gridZ);
void updateTiles(const glm::vec3& userPosition);
std::tuple<int, int> getCurrentGridPosition(const glm::vec3& position);

#endif // TILE_H
