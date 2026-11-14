//views/components/mapView.cpp
#include "views/components/mapView.hpp"

Color mapView::terrainColor(Terrain t) const {
    switch (t) {
        case Terrain::Plain:    return { 140, 190, 110, 255 };
        case Terrain::Road:     return { 200, 180, 130, 255 };
        case Terrain::Wood:     return {  45,  95,  55, 255 };
        case Terrain::Hill:     return { 150, 140, 100, 255 };
        case Terrain::Mountain: return { 110, 100,  95, 255 };
        case Terrain::Swamp:    return {  70,  90,  70, 255 };
        case Terrain::River:    return {  70, 130, 200, 255 };
    }
    return WHITE;
}

void mapView::drawCursor(int gx, int gy) const {
    int px = ox_ + gx * ts_;
    int py = oy_ + gy * ts_;
    // 1px inset so the cursor sits *inside* the tile, never on its neighbours.
    DrawRectangleLines(px + 1, py + 1, ts_ - 2, ts_ - 2, RAYWHITE);
    // Faint outer halo for visibility on light terrain.
    DrawRectangleLinesEx({ (float)px, (float)py, (float)ts_, (float)ts_ },
                         1.0f, Fade(RAYWHITE, 0.25f));
}

void mapView::draw(const Game& g, int hoverGx, int hoverGy) const {
    const Map& m = g.map();

    // 1) Terrain + tile borders.
    for (int y = 0; y < m.height(); ++y)
        for (int x = 0; x < m.width(); ++x) {
            int px = ox_ + x * ts_, py = oy_ + y * ts_;
            DrawRectangle(px, py, ts_, ts_, terrainColor(m.tileAt(x, y).terrain));
            DrawRectangleLines(px, py, ts_, ts_, Fade(BLACK, 0.15f));
        }

    // 2) Cursor on top of terrain (under units, which draw next).
    if (hoverGx >= 0 && hoverGy >= 0 && m.inBounds(hoverGx, hoverGy))
        drawCursor(hoverGx, hoverGy);
}