//views/components/hudView.cpp
#include "views/components/hudView.hpp"
#include "views/components/screenScaler.hpp"
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

static constexpr int HUD_X = 840;

void hudView::draw(const Game& g, const generalController& ctrl) const {
    DrawTextHi(g.mapName().c_str(), HUD_X, 20, 22, GOLD);

    const char* phaseTxt =
        ctrl.phase() == Phase::PlayerTurn ? "PLAYER TURN" :
        ctrl.phase() == Phase::EnemyTurn  ? "ENEMY TURN"  : "BATTLE OVER";
    DrawTextHi(phaseTxt, HUD_X, 50, 22, RAYWHITE);
    DrawTextHi(TextFormat("Turn %d", ctrl.turnNumber()), HUD_X, 78, 18, LIGHTGRAY);

    if (g.selectedUnitId != -1) {
        if (const UnitOwnInfantryLine* u = const_cast<Game&>(g).ownById(g.selectedUnitId)) {
            const Tile& t = g.map().tileAt(u->gridX, u->gridY);
            DrawTextHi(u->name.c_str(), HUD_X, 110, 22, YELLOW);
            DrawTextHi(TextFormat("HP %d/%d", u->hp, u->maxHp),
                       HUD_X, 138, 18, RAYWHITE);
            DrawTextHi(TextFormat("ATK %d  RNG %d", u->attackPower, u->attackRange),
                       HUD_X, 160, 18, RAYWHITE);
            DrawTextHi(TextFormat("On %s", terrainName(t.terrain)),
                       HUD_X, 182, 16, LIGHTGRAY);
            DrawTextHi(TextFormat("  atk %+d%%  def %+d%%",
                                  t.attackMod - 100, defBonus(t.defenseMod)),
                       HUD_X, 200, 14, DARKGRAY);
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
    int y = 240;

    DrawTextHi("CURSOR TILE", HUD_X, y, 12, DARKGRAY); y += 16;
    DrawTextHi(TextFormat("%d,%d  %s", hx, hy, terrainName(t.terrain)),
               HUD_X, y, 18, RAYWHITE); y += 22;
    DrawTextHi(terrainBlurb(t.terrain), HUD_X, y, 13, SKYBLUE); y += 18;
    DrawTextHi(TextFormat("move %d  |  atk %+d%%  def %+d%%",
                          t.moveCost, t.attackMod - 100, defBonus(t.defenseMod)),
               HUD_X, y, 13, LIGHTGRAY); y += 20;

    if (!t.walkable) {
        DrawTextHi("(impassable)", HUD_X, y, 13, ORANGE); y += 18;
    }

    if (const UnitOwnInfantryLine* u = ownAt(g, hx, hy)) {
        DrawTextHi(TextFormat("Friendly: %s  HP %d/%d",
                              u->name.c_str(), u->hp, u->maxHp),
                   HUD_X, y, 14, SKYBLUE); y += 18;
    } else if (const UnitEnemyInfantryLine* e = enemyAt(g, hx, hy)) {
        DrawTextHi(TextFormat("Hostile: %s  HP %d/%d",
                              e->name.c_str(), e->hp, e->maxHp),
                   HUD_X, y, 14, RED); y += 18;
    } else {
        DrawTextHi("(empty)", HUD_X, y, 13, DARKGRAY); y += 18;
    }

    y += 6;

    if (g.selectedUnitId == -1) return;

    const UnitOwnInfantryLine* sel = const_cast<Game&>(g).ownById(g.selectedUnitId);
    if (!sel) return;

    DrawTextHi("ACTION", HUD_X, y, 12, DARKGRAY); y += 16;

    const UnitEnemyInfantryLine* target = enemyAt(g, hx, hy);
    if (target) {
        int dist = std::abs(target->gridX - sel->gridX) +
                   std::abs(target->gridY - sel->gridY);
        if (dist > sel->attackRange) {
            DrawTextHi("Target out of range", HUD_X, y, 15, DARKGRAY);
            return;
        }

        int dealt = ctrl.previewDamage(sel->attackPower, sel->hp, sel->maxHp,
                                       sel->gridX, sel->gridY,
                                       target->hp, target->maxHp,
                                       target->gridX, target->gridY);
        int targetHpAfter = std::max(0, target->hp - dealt);

        DrawTextHi(TextFormat("Attack %s", target->name.c_str()),
                   HUD_X, y, 16, ORANGE); y += 20;
        DrawTextHi(TextFormat("  Deal %d  (%d -> %d)",
                              dealt, target->hp, targetHpAfter),
                   HUD_X, y, 14, ORANGE); y += 18;

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
                DrawTextHi(TextFormat("  Take %d  (%d -> %d)",
                                      backDealt, sel->hp, selHpAfter),
                           HUD_X, y, 14, PURPLE); y += 18;
            } else {
                DrawTextHi("  No counter (out of its range)",
                           HUD_X, y, 13, DARKGRAY); y += 16;
            }
        } else {
            DrawTextHi("  Target will be eliminated",
                       HUD_X, y, 13, GREEN); y += 16;
        }

        const Tile& atkT = g.map().tileAt(sel->gridX, sel->gridY);
        const Tile& defT = g.map().tileAt(target->gridX, target->gridY);
        DrawTextHi(TextFormat("Atk tile: %s (atk %+d%%)",
                              terrainName(atkT.terrain), atkT.attackMod - 100),
                   HUD_X, y, 13, DARKGRAY); y += 16;
        DrawTextHi(TextFormat("Def tile: %s (def %+d%%)",
                              terrainName(defT.terrain), defBonus(defT.defenseMod)),
                   HUD_X, y, 13, DARKGRAY); y += 16;
        return;
    }

    if (ownAt(g, hx, hy)) {
        DrawTextHi("Own unit (Act to reselect)", HUD_X, y, 14, YELLOW);
        return;
    }

    if (ctrl.isReachable(sel->gridX, sel->gridY, sel->moveRange, hx, hy)) {
        DrawTextHi(TextFormat("Move here (cost %d)", t.moveCost),
                   HUD_X, y, 15, SKYBLUE);
    } else {
        DrawTextHi("Not reachable", HUD_X, y, 14, DARKGRAY);
    }
}