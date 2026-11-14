//models/Map.hpp
#pragma once
#include <vector>

enum class Terrain {
    Plain,
    Road,
    Wood,
    Hill,
    Mountain,
    Swamp,
    River
};

struct Tile {
    Terrain terrain = Terrain::Plain;
    bool walkable = true;
    int  moveCost = 1;

    int attackMod  = 100;   // % multiplier applied to damage dealt from this tile
    int defenseMod = 100;   // % multiplier applied to damage taken on this tile
};

class Map {
public:
    Map(int w, int h);
    int width()  const { return width_; }
    int height() const { return height_; }
    const Tile& tileAt(int x, int y) const;
    Tile&       tileAt(int x, int y);
    bool inBounds(int x, int y) const;
private:
    int width_, height_;
    std::vector<Tile> tiles_;
};