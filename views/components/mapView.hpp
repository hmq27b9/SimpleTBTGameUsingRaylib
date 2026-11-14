//views/components/mapView.hpp
#pragma once
#include "models/Game.hpp"
#include "raylib.h"

class mapView {
public:
    mapView(int tileSize, int ox, int oy) : ts_(tileSize), ox_(ox), oy_(oy) {}

    // hoverGx/hoverGy = -1 means "no hover"; a cursor outline is drawn only
    // when both are >= 0 and inside the map.
    void draw(const Game& g, int hoverGx = -1, int hoverGy = -1) const;

    int tileSize() const { return ts_; }
    int originX()  const { return ox_; }
    int originY()  const { return oy_; }

private:
    Color terrainColor(Terrain) const;
    void  drawCursor(int gx, int gy) const;
    int ts_, ox_, oy_;
};