#pragma once
#include "model/GameModel.hpp"
#include "raylib.h"
class MapView {
public:
    MapView(int tileSize, int ox, int oy) : ts_(tileSize), ox_(ox), oy_(oy) {}
    void draw(const GameModel&) const;
    int tileSize() const { return ts_; }
    int originX()  const { return ox_; }
    int originY()  const { return oy_; }
private:
    Color terrainColor(Terrain) const;
    int ts_, ox_, oy_;
};