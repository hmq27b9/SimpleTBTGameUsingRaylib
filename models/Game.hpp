#pragma once
#include "models/Map.hpp"
#include "models/OwnInfantry.hpp"
#include <vector>
#include <string>

struct MapData;   // defined in controllers/generalController.hpp

class Game {
public:
    explicit Game(const MapData& data);

    Map& map() { return map_; }
    const Map& map() const { return map_; }
    std::vector<OwnInfantry>& infantry() { return infantry_; }
    const std::vector<OwnInfantry>& infantry() const { return infantry_; }
    const std::string& mapName() const { return mapName_; }

    OwnInfantry* infantryAt(int x, int y);
    const OwnInfantry* infantryAt(int x, int y) const;
    OwnInfantry* infantryById(int id);

    int selectedUnitId = -1;
    bool isGameOver() const;

private:
    Map map_;
    std::vector<OwnInfantry> infantry_;
    std::string mapId_, mapName_;
};