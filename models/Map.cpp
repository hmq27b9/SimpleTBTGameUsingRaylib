#include "models/Map.hpp"

Map::Map(int w, int h) : width_(w), height_(h), tiles_(w * h) {}
bool Map::inBounds(int x, int y) const {
    return x >= 0 && y >= 0 && x < width_ && y < height_;
}
const Tile& Map::tileAt(int x, int y) const { return tiles_[y * width_ + x]; }
Tile&       Map::tileAt(int x, int y)       { return tiles_[y * width_ + x]; }