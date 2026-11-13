//views/components/mapView.cpp
#include "views/components/mapView.hpp"

Color mapView::terrainColor(Terrain t) const {
    switch (t) {
        case Terrain::Grass:    return { 120, 180,  90, 255 };
        case Terrain::Forest:   return {  40, 100,  50, 255 };
        case Terrain::Water:    return {  60, 120, 200, 255 };
        case Terrain::Mountain: return { 130, 120, 110, 255 };
    }
    return WHITE;
}

void mapView::draw(const Game& g) const {
    const Map& m = g.map();
    for (int y = 0; y < m.height(); ++y)
        for (int x = 0; x < m.width(); ++x) {
            int px = ox_ + x * ts_, py = oy_ + y * ts_;
            DrawRectangle(px, py, ts_, ts_, terrainColor(m.tileAt(x, y).terrain));
            DrawRectangleLines(px, py, ts_, ts_, Fade(BLACK, 0.15f));
        }
}