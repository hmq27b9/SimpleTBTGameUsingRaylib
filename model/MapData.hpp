#pragma once
#include <string>
#include <vector>

struct TerrainSpawn { int x, y; char code; };  // '.' F ~ ^
struct UnitSpawn {
    std::string name; int x, y; bool player;
    int hp = 10, atk = 3, moveRange = 2, attackRange = 1;
};

struct MapData {
    std::string id;
    std::string displayName;
    int width = 0, height = 0;
    std::vector<TerrainSpawn> terrain;
    std::vector<UnitSpawn>    units;
};