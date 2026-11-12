#include "controller/BattleController.hpp"
#include <cstdlib>

bool BattleController::inMoveRange(const Unit& u, int gx, int gy) const {
    if (!model_.map().inBounds(gx, gy)) return false;
    if (!model_.map().tileAt(gx, gy).walkable) return false;
    int d = std::abs(gx - u.gridX) + std::abs(gy - u.gridY);
    return d > 0 && d <= u.moveRange;
}

void BattleController::onMouseClick(int sx, int sy, int ts, int ox, int oy) {
    handleTileClick((sx - ox) / ts, (sy - oy) / ts);
}

void BattleController::handleTileClick(int gx, int gy) {
    if (model_.turns().phase() != Phase::PlayerTurn) return;
    if (!model_.map().inBounds(gx, gy)) return;

    if (model_.selectedUnitId == -1) {
        Unit* u = model_.unitAt(gx, gy);
        if (u && u->isPlayerTeam && !u->hasActed)
            model_.selectedUnitId = u->id;
        return;
    }
    Unit* sel = model_.unitById(model_.selectedUnitId);
    if (!sel) { model_.selectedUnitId = -1; return; }
    Unit* target = model_.unitAt(gx, gy);
    if (target && target->isPlayerTeam != sel->isPlayerTeam) tryAttack(gx, gy);
    else if (!target)                                        tryMove(gx, gy);
    else                                                     model_.selectedUnitId = target->id;
}

void BattleController::tryMove(int gx, int gy) {
    Unit* sel = model_.unitById(model_.selectedUnitId);
    if (!sel || sel->hasMoved) return;
    if (!inMoveRange(*sel, gx, gy)) return;
    sel->gridX = gx; sel->gridY = gy;
    sel->hasMoved = true;
}

void BattleController::tryAttack(int gx, int gy) {
    Unit* sel = model_.unitById(model_.selectedUnitId);
    Unit* target = model_.unitAt(gx, gy);
    if (!sel || !target) return;
    if (std::abs(gx - sel->gridX) + std::abs(gy - sel->gridY) > sel->attackRange) return;
    target->hp -= sel->attackPower;
    sel->hasActed = true;
    model_.selectedUnitId = -1;
}

void BattleController::onEndTurnPressed() {
    if (model_.turns().phase() != Phase::PlayerTurn) return;
    for (auto& u : model_.units()) { u.hasMoved = false; u.hasActed = false; }
    model_.turns().endPlayerTurn();

    static const int dx[4]={1,-1,0,0}, dy[4]={0,0,1,-1};
    for (auto& e : model_.units()) {
        if (!e.isAlive() || e.isPlayerTeam) continue;
        for (int i=0;i<4;++i) {
            Unit* p = model_.unitAt(e.gridX+dx[i], e.gridY+dy[i]);
            if (p && p->isPlayerTeam) { p->hp -= e.attackPower; break; }
        }
    }
    model_.selectedUnitId = -1;
    model_.turns().endEnemyTurn();
    if (model_.isGameOver()) model_.turns().setBattleOver();
}