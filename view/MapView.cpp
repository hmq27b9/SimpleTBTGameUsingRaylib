#include "view/MapView.hpp"
Color MapView::terrainColor(Terrain t) const {
    switch (t) {
        case Terrain::Grass:    return { 120, 180,  90, 255 };
        case Terrain::Forest:   return {  40, 100,  50, 255 };
        case Terrain::Water:    return {  60, 120, 200, 255 };
        case Terrain::Mountain: return { 130, 120, 110, 255 };
    }
    return WHITE;
}
void MapView::draw(const GameModel& m) const {
    const Map& mp = m.map();
    for (int y = 0; y < mp.height(); ++y)
        for (int x = 0; x < mp.width(); ++x) {
            int px = ox_ + x*ts_, py = oy_ + y*ts_;
            DrawRectangle(px, py, ts_, ts_, terrainColor(mp.tileAt(x,y).terrain));
            DrawRectangleLines(px, py, ts_, ts_, Fade(BLACK, 0.15f));
        }
}