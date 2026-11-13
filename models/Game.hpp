//models/Game.hpp
#pragma once
#include "models/Map.hpp"
#include "models/OwnInfantry.hpp"
#include "models/EnemyInfantry.hpp"
#include <vector>
#include <string>

struct MapData;

class Game {
public:
    explicit Game(const MapData& data);

    Map& map() { return map_; }
    const Map& map() const { return map_; }

    std::vector<OwnInfantry>&   ownInfantry()   { return ownInfantry_; }
    const std::vector<OwnInfantry>& ownInfantry() const { return ownInfantry_; }

    std::vector<EnemyInfantry>& enemyInfantry() { return enemyInfantry_; }
    const std::vector<EnemyInfantry>& enemyInfantry() const { return enemyInfantry_; }

    const std::string& mapName() const { return mapName_; }

    OwnInfantry*   ownById(int id);
    EnemyInfantry* enemyById(int id);

    bool tileOccupied(int x, int y) const;

    int selectedUnitId = -1;
    bool isGameOver() const;

private:
    Map map_;
    std::vector<OwnInfantry>   ownInfantry_;
    std::vector<EnemyInfantry> enemyInfantry_;
    std::string mapId_, mapName_;
};