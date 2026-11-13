#include "controllers/generalController.hpp"
#include <cstdlib>

bool generalController::inMoveRange(const OwnInfantry& u, int gx, int gy) const {
    if (!game_.map().inBounds(gx, gy)) return false;
    if (!game_.map().tileAt(gx, gy).walkable) return false;
    int d = std::abs(gx - u.gridX) + std::abs(gy - u.gridY);
    return d > 0 && d <= u.moveRange;
}

void generalController::onMouseClick(int sx, int sy, int ts, int ox, int oy) {
    handleTileClick((sx - ox) / ts, (sy - oy) / ts);
}

void generalController::handleTileClick(int gx, int gy) {
    if (phase_ != Phase::PlayerTurn) return;
    if (!game_.map().inBounds(gx, gy)) return;

    if (game_.selectedUnitId == -1) {
        OwnInfantry* u = game_.infantryAt(gx, gy);
        if (u && u->isPlayerTeam && !u->hasActed)
            game_.selectedUnitId = u->id;
        return;
    }
    OwnInfantry* sel = game_.infantryById(game_.selectedUnitId);
    if (!sel) { game_.selectedUnitId = -1; return; }

    OwnInfantry* target = game_.infantryAt(gx, gy);
    if (target && target->isPlayerTeam != sel->isPlayerTeam) tryAttack(gx, gy);
    else if (!target)                                        tryMove(gx, gy);
    else                                                     game_.selectedUnitId = target->id;
}

void generalController::tryMove(int gx, int gy) {
    OwnInfantry* sel = game_.infantryById(game_.selectedUnitId);
    if (!sel || sel->hasMoved) return;
    if (!inMoveRange(*sel, gx, gy)) return;
    sel->gridX = gx; sel->gridY = gy;
    sel->hasMoved = true;
}

void generalController::tryAttack(int gx, int gy) {
    OwnInfantry* sel = game_.infantryById(game_.selectedUnitId);
    OwnInfantry* target = game_.infantryAt(gx, gy);
    if (!sel || !target) return;
    if (std::abs(gx - sel->gridX) + std::abs(gy - sel->gridY) > sel->attackRange) return;
    target->hp -= sel->attackPower;
    sel->hasActed = true;
    game_.selectedUnitId = -1;
}

void generalController::onEndTurnPressed() {
    if (phase_ != Phase::PlayerTurn) return;
    for (auto& u : game_.infantry()) { u.hasMoved = false; u.hasActed = false; }
    phase_ = Phase::EnemyTurn;
    runEnemyTurn();
    phase_ = Phase::PlayerTurn;
    ++turnNumber_;
    if (game_.isGameOver()) phase_ = Phase::BattleOver;
}

void generalController::runEnemyTurn() {
    static const int dx[4] = {1, -1, 0, 0};
    static const int dy[4] = {0, 0, 1, -1};
    for (auto& e : game_.infantry()) {
        if (!e.isAlive() || e.isPlayerTeam) continue;
        for (int i = 0; i < 4; ++i) {
            OwnInfantry* p = game_.infantryAt(e.gridX + dx[i], e.gridY + dy[i]);
            if (p && p->isPlayerTeam) { p->hp -= e.attackPower; break; }
        }
    }
    game_.selectedUnitId = -1;
}