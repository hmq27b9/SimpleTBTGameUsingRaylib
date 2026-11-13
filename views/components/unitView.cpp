#include "views/components/unitView.hpp"
#include <cstdlib>
#include <cmath>

// Draw a NATO-style infantry symbol (APP-6) centered at (cx, cy) with size s.
// Friendly units use blue, hostile use red.
static void drawNATOInfantry(float cx, float cy, float s, Color fill, Color line) {
    float half = s * 0.5f;
    Rectangle r = { cx - half, cy - half, s, s };

    // Filled frame
    DrawRectangleRec(r, fill);

    // Border
    DrawRectangleLinesEx(r, 2.0f, line);

    // "X" diagonals (infantry modifier)
    DrawLineEx({r.x, r.y},             {r.x + r.width, r.y + r.height}, 2.0f, line);
    DrawLineEx({r.x + r.width, r.y},   {r.x, r.y + r.height},           2.0f, line);
}

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

    const float symSize = ts * 0.7f;

    // Player units — friendly NATO infantry (blue)
    for (const auto& u : g.ownInfantry()) {
        if (!u.isAlive()) continue;
        int px = ox + u.gridX * ts, py = oy + u.gridY * ts;
        drawNATOInfantry(px + ts/2, py + ts/2, symSize,
                         Fade(BLUE, 0.85f), RAYWHITE);
        if (u.id == g.selectedUnitId) DrawRectangleLines(px, py, ts, ts, YELLOW);
        float f = (float)u.hp / u.maxHp;
        DrawRectangle(px + 4, py + ts - 8, (int)((ts - 8) * f), 4, GREEN);
    }

    // Enemy units — hostile NATO infantry (red)
    for (const auto& e : g.enemyInfantry()) {
        if (!e.isAlive()) continue;
        int px = ox + e.gridX * ts, py = oy + e.gridY * ts;
        drawNATOInfantry(px + ts/2, py + ts/2, symSize,
                         Fade(RED, 0.85f), RAYWHITE);
        float f = (float)e.hp / e.maxHp;
        DrawRectangle(px + 4, py + ts - 8, (int)((ts - 8) * f), 4, ORANGE);
    }
}