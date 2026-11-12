#pragma once
#include "model/Map.hpp"
#include "model/Unit.hpp"
#include "model/TurnManager.hpp"
#include "model/MapData.hpp"
#include <vector>

class GameModel {
public:
    explicit GameModel(const MapData& data);

    Map& map() { return map_; }  const Map& map() const { return map_; }
    std::vector<Unit>& units() { return units_; }
    const std::vector<Unit>& units() const { return units_; }
    TurnManager& turns() { return turns_; }
    const TurnManager& turns() const { return turns_; }
    const std::string& mapName() const { return mapName_; }

    Unit* unitAt(int x, int y);
    const Unit* unitAt(int x, int y) const;
    Unit* unitById(int id);

    int  selectedUnitId = -1;
    bool isGameOver() const;

private:
    Map map_;
    std::vector<Unit> units_;
    TurnManager turns_;
    std::string mapId_, mapName_;
};