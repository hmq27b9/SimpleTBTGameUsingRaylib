#include "views/components/unitView.hpp"
#include <cstdlib>

void unitView::draw(const Game& g, const mapView& mv) const {
    int ts = mv.tileSize(), ox = mv.originX(), oy = mv.originY();
    const OwnInfantry* sel = g.selectedUnitId != -1
        ? const_cast<Game&>(g).infantryById(g.selectedUnitId) : nullptr;

    if (sel) {
        for (int dy = -sel->moveRange; dy <= sel->moveRange; ++dy)
            for (int dx = -sel->moveRange; dx <= sel->moveRange; ++dx) {
                if (std::abs(dx) + std::abs(dy) > sel->moveRange) continue;
                int gx = sel->gridX + dx, gy = sel->gridY + dy;
                if (!g.map().inBounds(gx, gy)) continue;
                if (!g.map().tileAt(gx, gy).walkable) continue;
                DrawRectangle(ox + gx*ts, oy + gy*ts, ts, ts, Fade(SKYBLUE, 0.35f));
            }
    }

    for (const auto& u : g.infantry()) {
        if (!u.isAlive()) continue;
        int px = ox + u.gridX * ts, py = oy + u.gridY * ts;
        DrawCircle(px + ts/2, py + ts/2, ts * 0.35f, u.isPlayerTeam ? BLUE : RED);
        if (u.id == g.selectedUnitId) DrawRectangleLines(px, py, ts, ts, YELLOW);
        float f = (float)u.hp / u.maxHp;
        DrawRectangle(px + 4, py + ts - 8, (int)((ts - 8) * f), 4, GREEN);
    }
}