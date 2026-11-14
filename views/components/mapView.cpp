//views/components/mapView.cpp
#include "views/components/mapView.hpp"

Color mapView::terrainColor(Terrain t) const {
    switch (t) {
        case Terrain::Plain:    return { 140, 190, 110, 255 }; // light grass
        case Terrain::Road:     return { 200, 180, 130, 255 }; // tan
        case Terrain::Wood:     return {  45,  95,  55, 255 }; // deep green
        case Terrain::Hill:     return { 150, 140, 100, 255 }; // olive
        case Terrain::Mountain: return { 110, 100,  95, 255 }; // grey-brown
        case Terrain::Swamp:    return {  70,  90,  70, 255 }; // murky green
        case Terrain::River:    return {  70, 130, 200, 255 }; // blue
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