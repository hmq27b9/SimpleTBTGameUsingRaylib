#pragma once
#include <vector>

enum class Terrain { Grass, Forest, Water, Mountain };
struct Tile {
    Terrain terrain = Terrain::Grass;
    bool walkable = true;
    int moveCost = 1;
};

class Map {
public:
    Map(int w, int h);
    int width()  const { return width_; }
    int height() const { return height_; }
    const Tile& tileAt(int x, int y) const;
    Tile& tileAt(int x, int y);
    bool inBounds(int x, int y) const;
private:
    int width_, height_;
    std::vector<Tile> tiles_;
};