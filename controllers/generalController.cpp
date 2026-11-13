//controllers/generalController.cpp
#include "controllers/generalController.hpp"
#include <cstdlib>
#include <vector>

bool generalController::inMoveRange(int gx, int gy, int moveRange,
                                    int tx, int ty) const {
    if (!game_.map().inBounds(tx, ty)) return false;
    if (!game_.map().tileAt(tx, ty).walkable) return false;
    if (game_.tileOccupied(tx, ty)) return false;
    int d = std::abs(tx - gx) + std::abs(ty - gy);
    return d > 0 && d <= moveRange;
}

void generalController::onMouseClick(int sx, int sy, int ts, int ox, int oy) {
    handleTileClick((sx - ox) / ts, (sy - oy) / ts);
}

void generalController::handleTileClick(int gx, int gy) {
    if (phase_ != Phase::PlayerTurn) return;
    if (!game_.map().inBounds(gx, gy)) return;

    // 1) Nothing selected → try to select a player unit at this tile
    if (game_.selectedUnitId == -1) {
        for (auto& u : game_.ownInfantry())
            if (u.isAlive() && u.gridX == gx && u.gridY == gy && !u.hasActed) {
                game_.selectedUnitId = u.id;
                return;
            }
        return;
    }

    UnitOwnInfantryLine* sel = game_.ownById(game_.selectedUnitId);
    if (!sel) { game_.selectedUnitId = -1; return; }

    // 2) Clicked an enemy → attack
    for (auto& e : game_.enemyInfantry())
        if (e.isAlive() && e.gridX == gx && e.gridY == gy) {
            tryAttack(gx, gy);
            return;
        }

    // 3) Clicked an own unit → reselect
    for (auto& u : game_.ownInfantry())
        if (u.isAlive() && u.gridX == gx && u.gridY == gy) {
            game_.selectedUnitId = u.id;
            return;
        }

    // 4) Empty tile → move
    tryMove(gx, gy);
}

void generalController::tryMove(int gx, int gy) {
    UnitOwnInfantryLine* sel = game_.ownById(game_.selectedUnitId);
    if (!sel || sel->hasMoved) return;
    if (!inMoveRange(sel->gridX, sel->gridY, sel->moveRange, gx, gy)) return;
    sel->gridX = gx; sel->gridY = gy;
    sel->hasMoved = true;
}

void generalController::tryAttack(int gx, int gy) {
    UnitOwnInfantryLine* sel = game_.ownById(game_.selectedUnitId);
    if (!sel) return;
    UnitEnemyInfantryLine* target = nullptr;
    for (auto& e : game_.enemyInfantry())
        if (e.isAlive() && e.gridX == gx && e.gridY == gy) { target = &e; break; }
    if (!target) return;
    if (std::abs(gx - sel->gridX) + std::abs(gy - sel->gridY) > sel->attackRange) return;
    target->hp -= sel->attackPower;
    sel->hasActed = true;
    game_.selectedUnitId = -1;
}

// ---- Enemy turn -------------------------------------------------

bool generalController::tryEnemyStep(UnitEnemyInfantryLine& e) {
    // Nearest living player
    const UnitOwnInfantryLine* nearest = nullptr;
    int bestDist = 9999;
    for (const auto& p : game_.ownInfantry()) {
        if (!p.isAlive()) continue;
        int d = std::abs(p.gridX - e.gridX) + std::abs(p.gridY - e.gridY);
        if (d < bestDist) { bestDist = d; nearest = &p; }
    }
    if (!nearest || bestDist == 1) return false;

    int dx = (nearest->gridX > e.gridX) - (nearest->gridX < e.gridX);
    int dy = (nearest->gridY > e.gridY) - (nearest->gridY < e.gridY);

    struct Step { int x, y; };
    std::vector<Step> candidates;
    if (std::abs(nearest->gridX - e.gridX) >= std::abs(nearest->gridY - e.gridY)) {
        if (dx) candidates.push_back({ e.gridX + dx, e.gridY });
        if (dy) candidates.push_back({ e.gridX, e.gridY + dy });
    } else {
        if (dy) candidates.push_back({ e.gridX, e.gridY + dy });
        if (dx) candidates.push_back({ e.gridX + dx, e.gridY });
    }
    for (auto& c : candidates) {
        if (!game_.map().inBounds(c.x, c.y)) continue;
        if (!game_.map().tileAt(c.x, c.y).walkable) continue;
        if (game_.tileOccupied(c.x, c.y)) continue;
        e.gridX = c.x; e.gridY = c.y;
        return true;
    }
    return false;
}

void generalController::runEnemyTurn() {
    for (auto& e : game_.enemyInfantry()) {
        if (!e.isAlive()) continue;
        for (int step = 0; step < e.moveRange; ++step)
            if (!tryEnemyStep(e)) break;

        static const int dx[4] = {1, -1, 0, 0};
        static const int dy[4] = {0, 0, 1, -1};
        for (int i = 0; i < 4; ++i) {
            for (auto& p : game_.ownInfantry())
                if (p.isAlive() &&
                    p.gridX == e.gridX + dx[i] &&
                    p.gridY == e.gridY + dy[i]) {
                    p.hp -= e.attackPower;
                    goto next_enemy;
                }
        }
        next_enemy:;
    }
    game_.selectedUnitId = -1;
}

void generalController::onEndTurnPressed() {
    if (phase_ != Phase::PlayerTurn) return;
    for (auto& u : game_.ownInfantry())   { u.hasMoved = false; u.hasActed = false; }
    for (auto& e : game_.enemyInfantry()) { e.hasMoved = false; e.hasActed = false; }
    phase_ = Phase::EnemyTurn;
    runEnemyTurn();
    phase_ = Phase::PlayerTurn;
    ++turnNumber_;
    if (game_.isGameOver()) phase_ = Phase::BattleOver;
}