//views/components/hudView.cpp
#include "views/components/hudView.hpp"
#include <cstdlib>
#include <algorithm>

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

static const char* terrainBlurb(Terrain t) {
    switch (t) {
        case Terrain::Plain:    return "Open ground - no modifiers";
        case Terrain::Road:     return "Fast but exposed";
        case Terrain::Wood:     return "Defensive cover - hard to hit";
        case Terrain::Hill:     return "High ground - good attack, exposed";
        case Terrain::Mountain: return "Impassable ridge";
        case Terrain::Swamp:    return "Slow going, hard to hit";
        case Terrain::River:    return "Impassable water";
    }
    return "";
}

static int defBonus(int defenseMod) { return 100 - defenseMod; }

void hudView::draw(const Game& g, const generalController& ctrl) const {
    DrawText(g.mapName().c_str(), 840, 20, 22, GOLD);

    const char* phaseTxt =
        ctrl.phase() == Phase::PlayerTurn ? "PLAYER TURN" :
        ctrl.phase() == Phase::EnemyTurn  ? "ENEMY TURN"  : "BATTLE OVER";
    DrawText(phaseTxt, 840, 50, 22, RAYWHITE);
    DrawText(TextFormat("Turn %d", ctrl.turnNumber()), 840, 78, 18, LIGHTGRAY);

    DrawText("[ARROWS] Move cursor", 840, 104, 14, LIGHTGRAY);
    DrawText("[ENTER/SPACE] Act", 840, 122, 14, LIGHTGRAY);
    DrawText("[TAB] Next unit   [ESC] Deselect", 840, 140, 14, LIGHTGRAY);
    DrawText("[E] End turn", 840, 158, 14, LIGHTGRAY);
    DrawText("[F1/O] Settings   [F11] Fullscreen", 840, 176, 14, SKYBLUE);

    if (g.selectedUnitId != -1) {
        if (const UnitOwnInfantryLine* u = const_cast<Game&>(g).ownById(g.selectedUnitId)) {
            const Tile& t = g.map().tileAt(u->gridX, u->gridY);
            DrawText(u->name.c_str(), 840, 200, 22, YELLOW);
            DrawText(TextFormat("HP %d/%d", u->hp, u->maxHp), 840, 228, 18, RAYWHITE);
            DrawText(TextFormat("ATK %d  RNG %d", u->attackPower, u->attackRange),
                     840, 250, 18, RAYWHITE);
            DrawText(TextFormat("On %s", terrainName(t.terrain)),
                     840, 272, 16, LIGHTGRAY);
            DrawText(TextFormat("  atk %+d%%  def %+d%%",
                                t.attackMod - 100, defBonus(t.defenseMod)),
                     840, 290, 14, DARKGRAY);
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

    const Tile& t = g.map().tileAt(hx, hy);
    int y = 320;

    DrawText("CURSOR TILE", 840, y, 12, DARKGRAY); y += 16;
    DrawText(TextFormat("%d,%d  %s", hx, hy, terrainName(t.terrain)),
             840, y, 18, RAYWHITE); y += 22;
    DrawText(terrainBlurb(t.terrain), 840, y, 13, SKYBLUE); y += 18;
    DrawText(TextFormat("move %d  |  atk %+d%%  def %+d%%",
                        t.moveCost, t.attackMod - 100, defBonus(t.defenseMod)),
             840, y, 13, LIGHTGRAY); y += 20;

    if (!t.walkable) {
        DrawText("(impassable)", 840, y, 13, ORANGE); y += 18;
    }

    if (const UnitOwnInfantryLine* u = ownAt(g, hx, hy)) {
        DrawText(TextFormat("Friendly: %s  HP %d/%d", u->name.c_str(), u->hp, u->maxHp),
                 840, y, 14, SKYBLUE); y += 18;
    } else if (const UnitEnemyInfantryLine* e = enemyAt(g, hx, hy)) {
        DrawText(TextFormat("Hostile: %s  HP %d/%d", e->name.c_str(), e->hp, e->maxHp),
                 840, y, 14, RED); y += 18;
    } else {
        DrawText("(empty)", 840, y, 13, DARKGRAY); y += 18;
    }

    y += 6;

    if (g.selectedUnitId == -1) return;

    const UnitOwnInfantryLine* sel = const_cast<Game&>(g).ownById(g.selectedUnitId);
    if (!sel) return;

    DrawText("ACTION", 840, y, 12, DARKGRAY); y += 16;

    const UnitEnemyInfantryLine* target = enemyAt(g, hx, hy);
    if (target) {
        int dist = std::abs(target->gridX - sel->gridX) +
                   std::abs(target->gridY - sel->gridY);
        if (dist > sel->attackRange) {
            DrawText("Target out of range", 840, y, 15, DARKGRAY);
            return;
        }

        int dealt = ctrl.previewDamage(sel->attackPower, sel->hp, sel->maxHp,
                                       sel->gridX, sel->gridY,
                                       target->hp, target->maxHp,
                                       target->gridX, target->gridY);
        int targetHpAfter = std::max(0, target->hp - dealt);

        DrawText(TextFormat("Attack %s", target->name.c_str()), 840, y, 16, ORANGE);
        y += 20;
        DrawText(TextFormat("  Deal %d  (%d -> %d)",
                            dealt, target->hp, targetHpAfter),
                 840, y, 14, ORANGE); y += 18;

        if (targetHpAfter > 0) {
            int counterDist = std::abs(sel->gridX - target->gridX) +
                              std::abs(sel->gridY - target->gridY);
            if (counterDist <= target->attackRange) {
                int backDealt = ctrl.previewDamage(target->attackPower,
                                                   targetHpAfter, target->maxHp,
                                                   target->gridX, target->gridY,
                                                   sel->hp, sel->maxHp,
                                                   sel->gridX, sel->gridY);
                int selHpAfter = std::max(0, sel->hp - backDealt);
                DrawText(TextFormat("  Take %d  (%d -> %d)",
                                    backDealt, sel->hp, selHpAfter),
                         840, y, 14, PURPLE); y += 18;
            } else {
                DrawText("  No counter (out of its range)", 840, y, 13, DARKGRAY); y += 16;
            }
        } else {
            DrawText("  Target will be eliminated", 840, y, 13, GREEN); y += 16;
        }

        const Tile& atkT = g.map().tileAt(sel->gridX, sel->gridY);
        const Tile& defT = g.map().tileAt(target->gridX, target->gridY);
        DrawText(TextFormat("Atk tile: %s (atk %+d%%)",
                            terrainName(atkT.terrain), atkT.attackMod - 100),
                 840, y, 13, DARKGRAY); y += 16;
        DrawText(TextFormat("Def tile: %s (def %+d%%)",
                            terrainName(defT.terrain), defBonus(defT.defenseMod)),
                 840, y, 13, DARKGRAY); y += 16;
        return;
    }

    if (ownAt(g, hx, hy)) {
        DrawText("Own unit (Act to reselect)", 840, y, 14, YELLOW);
        return;
    }

    if (ctrl.isReachable(sel->gridX, sel->gridY, sel->moveRange, hx, hy)) {
        DrawText(TextFormat("Move here (cost %d)", t.moveCost), 840, y, 15, SKYBLUE);
    } else {
        DrawText("Not reachable", 840, y, 14, DARKGRAY);
    }
}