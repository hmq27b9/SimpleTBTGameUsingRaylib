//views/components/hudView.cpp
#include "views/components/hudView.hpp"
#include <cstdlib>

static const char* terrainName(Terrain t) {
    switch (t) {
        case Terrain::Plain:    return "Plain";
        case Terrain::Road:     return "Road";
        case Terrain::Wood:     return "Wood";
        case Terrain::Hill:     return "Hill";
        case Terrain::Mountain: return "Mountain";
        case Terrain::Swamp:    return "Swamp";
        case Terrain::River:    return "River";
    }
    return "?";
}

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
            const Tile& t = g.map().tileAt(u->gridX, u->gridY);
            DrawText(u->name.c_str(), 820, 160, 22, YELLOW);
            DrawText(TextFormat("HP %d/%d", u->hp, u->maxHp), 820, 188, 18, RAYWHITE);
            DrawText(TextFormat("ATK %d  RNG %d", u->attackPower, u->attackRange),
                     820, 210, 18, RAYWHITE);
            DrawText(TextFormat("Terrain: %s", terrainName(t.terrain)),
                     820, 232, 16, LIGHTGRAY);
            DrawText(TextFormat("  atk x%d%%  def x%d%%",
                                t.attackMod, t.defenseMod),
                     820, 250, 14, DARKGRAY);
        }
    }
}

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

    if (g.selectedUnitId == -1) {
        if (const UnitOwnInfantryLine* u = ownAt(g, hx, hy)) {
            const Tile& t = g.map().tileAt(hx, hy);
            DrawText(u->name.c_str(), 820, 260, 20, YELLOW);
            DrawText(TextFormat("HP %d/%d", u->hp, u->maxHp), 820, 286, 16, LIGHTGRAY);
            DrawText(TextFormat("ATK %d  RNG %d", u->attackPower, u->attackRange),
                     820, 308, 16, LIGHTGRAY);
            DrawText(TextFormat("On %s  (atk x%d%% / def x%d%%)",
                                terrainName(t.terrain), t.attackMod, t.defenseMod),
                     820, 330, 14, SKYBLUE);
            DrawText("Click to select", 820, 350, 14, DARKGRAY);
        }
        return;
    }

    const UnitOwnInfantryLine* sel = const_cast<Game&>(g).ownById(g.selectedUnitId);
    if (!sel) return;

    const UnitEnemyInfantryLine* target = enemyAt(g, hx, hy);
    if (!target) {
        if (!ownAt(g, hx, hy)) {
            if (ctrl.isReachable(sel->gridX, sel->gridY, sel->moveRange, hx, hy)) {
                const Tile& t = g.map().tileAt(hx, hy);
                DrawText(TextFormat("Move to %s (cost %d)",
                                    terrainName(t.terrain), t.moveCost),
                         820, 260, 16, SKYBLUE);
                if (t.attackMod != 100 || t.defenseMod != 100)
                    DrawText(TextFormat("  atk x%d%% / def x%d%%",
                                        t.attackMod, t.defenseMod),
                             820, 282, 14, DARKGRAY);
            }
        }
        return;
    }

    int dist = std::abs(target->gridX - sel->gridX) +
               std::abs(target->gridY - sel->gridY);
    if (dist > sel->attackRange) {
        DrawText("Target out of range", 820, 260, 16, DARKGRAY);
        return;
    }

    // --- attack forecast, all after terrain ---
    int dealt = ctrl.previewDamage(sel->attackPower, sel->hp, sel->maxHp,
                                   sel->gridX, sel->gridY,
                                   target->hp, target->maxHp,
                                   target->gridX, target->gridY);
    int targetHpAfter = std::max(0, target->hp - dealt);

    const Tile& atkT = g.map().tileAt(sel->gridX, sel->gridY);
    const Tile& defT = g.map().tileAt(target->gridX, target->gridY);

    DrawText(target->name.c_str(), 820, 260, 20, RED);
    DrawText(TextFormat("HP %d/%d", target->hp, target->maxHp), 820, 286, 16, LIGHTGRAY);
    DrawText(TextFormat("On %s  def x%d%%",
                        terrainName(defT.terrain), defT.defenseMod),
             820, 308, 14, SKYBLUE);

    DrawText(TextFormat("Deal %d  (%d -> %d)",
                        dealt, target->hp, targetHpAfter),
             820, 330, 16, ORANGE);

    if (targetHpAfter > 0) {
        int counterDist = std::abs(sel->gridX - target->gridX) +
                          std::abs(sel->gridY - target->gridY);
        if (counterDist <= target->attackRange) {
            // Counter uses the post-hit HP for the defender's attack scaling.
            int backDealt = ctrl.previewDamage(target->attackPower,
                                               targetHpAfter, target->maxHp,
                                               target->gridX, target->gridY,
                                               sel->hp, sel->maxHp,
                                               sel->gridX, sel->gridY);
            int selHpAfter = std::max(0, sel->hp - backDealt);
            DrawText(TextFormat("Take %d  (%d -> %d)",
                                backDealt, sel->hp, selHpAfter),
                     820, 354, 16, PURPLE);
        } else {
            DrawText("No counter (out of its range)", 820, 354, 14, DARKGRAY);
        }
    } else {
        DrawText("Target will be eliminated", 820, 354, 14, GREEN);
    }

    // Who is standing on what — the "why" of the numbers.
    DrawText(TextFormat("Atk tile: %s (x%d%%)",
                        terrainName(atkT.terrain), atkT.attackMod),
             820, 378, 13, DARKGRAY);
    DrawText(TextFormat("Def tile: %s (x%d%%)",
                        terrainName(defT.terrain), defT.defenseMod),
             820, 396, 13, DARKGRAY);
}