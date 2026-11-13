//views/components/mapView.hpp
#pragma once
#include "models/Game.hpp"
#include "raylib.h"

class mapView {
public:
    mapView(int tileSize, int ox, int oy) : ts_(tileSize), ox_(ox), oy_(oy) {}
    void draw(const Game& g) const;
    int tileSize() const { return ts_; }
    int originX()  const { return ox_; }
    int originY()  const { return oy_; }
private:
    Color terrainColor(Terrain) const;
    int ts_, ox_, oy_;
};