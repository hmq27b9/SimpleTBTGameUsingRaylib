//views/components/unitView.cpp
#include "views/components/unitView.hpp"
#include <cstdlib>

static void drawNATOInfantry(float cx, float cy, float s, Color fill, Color line) {
    float half = s * 0.5f;
    Rectangle r = { cx - half, cy - half, s, s };
    DrawRectangleRec(r, fill);
    DrawRectangleLinesEx(r, 2.0f, line);
    DrawLineEx({r.x, r.y},           {r.x + r.width, r.y + r.height}, 2.0f, line);
    DrawLineEx({r.x + r.width, r.y}, {r.x, r.y + r.height},           2.0f, line);
}

void unitView::draw(const Game& g, const mapView& mv,
                    const generalController& ctrl) const {
    int ts = mv.tileSize(), ox = mv.originX(), oy = mv.originY();

    // Move-range highlight — sourced from the controller's Dijkstra, so it
    // matches what tryMove() actually allows.
    if (g.selectedUnitId != -1) {
        for (const auto& u : g.ownInfantry()) {
            if (u.id != g.selectedUnitId || !u.isAlive() || u.hasMoved) continue;
            for (auto& p : ctrl.reachableTiles(u.gridX, u.gridY, u.moveRange)) {
                int gx = p.first, gy = p.second;
                const Tile& t = g.map().tileAt(gx, gy);
                // Tint the highlight by tile cost: cheap = blue, costly = warm.
                Color c = Fade(t.moveCost <= 1 ? SKYBLUE : 
                               t.moveCost == 2 ? ORANGE  : DARKPURPLE, 0.35f);
                DrawRectangle(ox + gx*ts, oy + gy*ts, ts, ts, c);
            }
        }
    }

    const float symSize = ts * 0.7f;

    for (const auto& u : g.ownInfantry()) {
        if (!u.isAlive()) continue;
        int px = ox + u.gridX * ts, py = oy + u.gridY * ts;
        drawNATOInfantry(px + ts/2, py + ts/2, symSize,
                         Fade(BLUE, 0.85f), RAYWHITE);
        if (u.id == g.selectedUnitId) DrawRectangleLines(px, py, ts, ts, YELLOW);
        float f = (float)u.hp / u.maxHp;
        DrawRectangle(px + 4, py + ts - 8, (int)((ts - 8) * f), 4, GREEN);
    }

    for (const auto& e : g.enemyInfantry()) {
        if (!e.isAlive()) continue;
        int px = ox + e.gridX * ts, py = oy + e.gridY * ts;
        drawNATOInfantry(px + ts/2, py + ts/2, symSize,
                         Fade(RED, 0.85f), RAYWHITE);
        float f = (float)e.hp / e.maxHp;
        DrawRectangle(px + 4, py + ts - 8, (int)((ts - 8) * f), 4, ORANGE);
    }
}