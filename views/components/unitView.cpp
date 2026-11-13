#include "views/components/unitView.hpp"
#include <cstdlib>

void unitView::draw(const Game& g, const mapView& mv) const {
    int ts = mv.tileSize(), ox = mv.originX(), oy = mv.originY();

    // Move-range highlight (player only)
    if (g.selectedUnitId != -1) {
        for (const auto& u : g.ownInfantry()) {
            if (u.id != g.selectedUnitId || !u.isAlive()) continue;
            for (int dy = -u.moveRange; dy <= u.moveRange; ++dy)
                for (int dx = -u.moveRange; dx <= u.moveRange; ++dx) {
                    if (std::abs(dx) + std::abs(dy) > u.moveRange) continue;
                    int gx = u.gridX + dx, gy = u.gridY + dy;
                    if (!g.map().inBounds(gx, gy)) continue;
                    if (!g.map().tileAt(gx, gy).walkable) continue;
                    if (g.tileOccupied(gx, gy)) continue;
                    DrawRectangle(ox + gx*ts, oy + gy*ts, ts, ts, Fade(SKYBLUE, 0.35f));
                }
        }
    }

    // Player units — blue circles
    for (const auto& u : g.ownInfantry()) {
        if (!u.isAlive()) continue;
        int px = ox + u.gridX * ts, py = oy + u.gridY * ts;
        DrawCircle(px + ts/2, py + ts/2, ts * 0.35f, BLUE);
        if (u.id == g.selectedUnitId) DrawRectangleLines(px, py, ts, ts, YELLOW);
        float f = (float)u.hp / u.maxHp;
        DrawRectangle(px + 4, py + ts - 8, (int)((ts - 8) * f), 4, GREEN);
    }

    // Enemy units — red circles
    for (const auto& e : g.enemyInfantry()) {
        if (!e.isAlive()) continue;
        int px = ox + e.gridX * ts, py = oy + e.gridY * ts;
        DrawCircle(px + ts/2, py + ts/2, ts * 0.35f, RED);
        float f = (float)e.hp / e.maxHp;
        DrawRectangle(px + 4, py + ts - 8, (int)((ts - 8) * f), 4, ORANGE);
    }
}