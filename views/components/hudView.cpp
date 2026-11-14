//views/components/hudView.cpp
#include "views/components/hudView.hpp"
#include <cstdlib>

void hudView::draw(const Game& g, const generalController& ctrl) const {
    DrawText(g.mapName().c_str(), 820, 20, 22, GOLD);

    const char* phaseTxt =
        ctrl.phase() == Phase::PlayerTurn ? "PLAYER TURN" :
        ctrl.phase() == Phase::EnemyTurn  ? "ENEMY TURN"  : "BATTLE OVER";
    DrawText(phaseTxt, 820, 50, 22, RAYWHITE);
    DrawText(TextFormat("Turn %d", ctrl.turnNumber()), 820, 78, 18, LIGHTGRAY);
    DrawText("[SPACE] End Turn", 820, 110, 16, LIGHTGRAY);

    if (g.selectedUnitId != -1) {
        if (const UnitOwnInfantryLine* u = const_cast<Game&>(g).ownById(g.selectedUnitId)) {
            DrawText(u->name.c_str(), 820, 160, 22, YELLOW);
            DrawText(TextFormat("HP %d/%d", u->hp, u->maxHp), 820, 188, 18, RAYWHITE);
            DrawText(TextFormat("ATK %d  RNG %d", u->attackPower, u->attackRange),
                     820, 210, 18, RAYWHITE);
        }
    }
}

// Helper: find live enemy / own unit at a grid tile.
static const UnitEnemyInfantryLine* enemyAt(const Game& g, int x, int y) {
    for (const auto& e : g.enemyInfantry())
        if (e.isAlive() && e.gridX == x && e.gridY == y) return &e;
    return nullptr;
}
static const UnitOwnInfantryLine* ownAt(const Game& g, int x, int y) {
    for (const auto& u : g.ownInfantry())
        if (u.isAlive() && u.gridX == x && u.gridY == y) return &u;
    return nullptr;
}

void hudView::drawHoverPreview(const Game& g,
                               const generalController& ctrl,
                               int hx, int hy) const {
    if (ctrl.phase() != Phase::PlayerTurn) return;
    if (!g.map().inBounds(hx, hy)) return;

    // Case A: nothing selected, hovering own unit → preview its stats.
    if (g.selectedUnitId == -1) {
        if (const UnitOwnInfantryLine* u = ownAt(g, hx, hy)) {
            DrawText(u->name.c_str(), 820, 260, 20, YELLOW);
            DrawText(TextFormat("HP %d/%d", u->hp, u->maxHp), 820, 286, 16, LIGHTGRAY);
            DrawText(TextFormat("ATK %d  RNG %d", u->attackPower, u->attackRange),
                     820, 308, 16, LIGHTGRAY);
            DrawText("Click to select", 820, 330, 14, DARKGRAY);
        }
        return;
    }

    // Case B: a unit is selected. If hovering an enemy in range, preview trade.
    const UnitOwnInfantryLine* sel = const_cast<Game&>(g).ownById(g.selectedUnitId);
    if (!sel) return;

    const UnitEnemyInfantryLine* target = enemyAt(g, hx, hy);
    if (!target) {
        // Hovering empty tile → show move-range indicator text.
        if (!ownAt(g, hx, hy)) {
            int d = std::abs(hx - sel->gridX) + std::abs(hy - sel->gridY);
            if (d > 0 && d <= sel->moveRange && !g.tileOccupied(hx, hy))
                DrawText("Click to move", 820, 260, 16, SKYBLUE);
        }
        return;
    }

    int dist = std::abs(target->gridX - sel->gridX) +
               std::abs(target->gridY - sel->gridY);
    if (dist > sel->attackRange) {
        DrawText("Target out of range", 820, 260, 16, DARKGRAY);
        return;
    }

    // --- Expected damage dealt by the selected unit ---
    int rawOut    = Game::damageFrom(sel->attackPower, sel->hp, sel->maxHp);
    int dealt     = Game::damageTakenBy(rawOut, target->hp, target->maxHp);
    int targetHpAfter = target->hp - dealt;

    DrawText(target->name.c_str(), 820, 260, 20, RED);
    DrawText(TextFormat("HP %d/%d", target->hp, target->maxHp), 820, 286, 16, LIGHTGRAY);

    DrawText(TextFormat("Deal %d  (%d -> %d)",
                        dealt, target->hp, targetHpAfter > 0 ? targetHpAfter : 0),
             820, 312, 16, ORANGE);

    // --- Counter-attack preview (only if target survives) ---
    if (targetHpAfter > 0) {
        int counterDist = std::abs(sel->gridX - target->gridX) +
                          std::abs(sel->gridY - target->gridY);
        if (counterDist <= target->attackRange) {
            int rawBack   = Game::damageFrom(target->attackPower, targetHpAfter, target->maxHp);
            int backDealt = Game::damageTakenBy(rawBack, sel->hp, sel->maxHp);
            int selHpAfter = sel->hp - backDealt;
            DrawText(TextFormat("Take %d  (%d -> %d)",
                                backDealt, sel->hp, selHpAfter > 0 ? selHpAfter : 0),
                     820, 336, 16, PURPLE);
        } else {
            DrawText("No counter (out of its range)", 820, 336, 14, DARKGRAY);
        }
    } else {
        DrawText("Target will be eliminated", 820, 336, 14, GREEN);
    }
}