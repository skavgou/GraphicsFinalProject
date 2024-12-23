#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <tuple>
#include "Building.h"

// Declare global variables as extern
extern glm::vec3 userPosition;
extern std::unordered_map<std::tuple<int, int>, std::vector<Building>> activeTiles;

#endif // GLOBAL_VARIABLES_H
