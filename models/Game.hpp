//models/Game.hpp
#pragma once
#include "models/Map.hpp"
#include "models/UnitOwnInfantryLine.hpp"
#include "models/UnitEnemyInfantryLine.hpp"
#include <vector>
#include <string>

struct MapData;

class Game {
public:
    explicit Game(const MapData& data);

    Map& map() { return map_; }
    const Map& map() const { return map_; }

    std::vector<UnitOwnInfantryLine>&   ownInfantry()   { return ownInfantry_; }
    const std::vector<UnitOwnInfantryLine>& ownInfantry() const { return ownInfantry_; }

    std::vector<UnitEnemyInfantryLine>& enemyInfantry() { return enemyInfantry_; }
    const std::vector<UnitEnemyInfantryLine>& enemyInfantry() const { return enemyInfantry_; }

    const std::string& mapName() const { return mapName_; }

    UnitOwnInfantryLine*   ownById(int id);
    UnitEnemyInfantryLine* enemyById(int id);

    bool tileOccupied(int x, int y) const;

    int selectedUnitId = -1;
    bool isGameOver() const;

private:
    Map map_;
    std::vector<UnitOwnInfantryLine>   ownInfantry_;
    std::vector<UnitEnemyInfantryLine> enemyInfantry_;
    std::string mapId_, mapName_;
};