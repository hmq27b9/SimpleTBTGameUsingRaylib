#include "view/UnitView.hpp"
#include <cstdlib>
void UnitView::draw(const GameModel& m, const MapView& mv) const {
    int ts = mv.tileSize(), ox = mv.originX(), oy = mv.originY();
    const Unit* sel = m.selectedUnitId != -1
        ? const_cast<GameModel&>(m).unitById(m.selectedUnitId) : nullptr;
    if (sel) {
        for (int dy = -sel->moveRange; dy <= sel->moveRange; ++dy)
            for (int dx = -sel->moveRange; dx <= sel->moveRange; ++dx) {
                if (std::abs(dx)+std::abs(dy) > sel->moveRange) continue;
                int gx = sel->gridX+dx, gy = sel->gridY+dy;
                if (!m.map().inBounds(gx,gy) || !m.map().tileAt(gx,gy).walkable) continue;
                DrawRectangle(ox + gx*ts, oy + gy*ts, ts, ts, Fade(SKYBLUE, 0.35f));
            }
    }
    for (const auto& u : m.units()) {
        if (!u.isAlive()) continue;
        int px = ox + u.gridX*ts, py = oy + u.gridY*ts;
        DrawCircle(px + ts/2, py + ts/2, ts*0.35f, u.isPlayerTeam ? BLUE : RED);
        if (u.id == m.selectedUnitId) DrawRectangleLines(px, py, ts, ts, YELLOW);
        float f = (float)u.hp / u.maxHp;
        DrawRectangle(px + 4, py + ts - 8, (int)((ts-8)*f), 4, GREEN);
    }
}