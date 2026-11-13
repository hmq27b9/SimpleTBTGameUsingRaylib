#include "views/components/hudView.hpp"

void hudView::draw(const Game& g, const generalController& ctrl) const {
    DrawText(g.mapName().c_str(), 820, 20, 22, GOLD);

    const char* phaseTxt =
        ctrl.phase() == Phase::PlayerTurn ? "PLAYER TURN" :
        ctrl.phase() == Phase::EnemyTurn  ? "ENEMY TURN"  : "BATTLE OVER";
    DrawText(phaseTxt, 820, 50, 22, RAYWHITE);
    DrawText(TextFormat("Turn %d", ctrl.turnNumber()), 820, 78, 18, LIGHTGRAY);
    DrawText("[SPACE] End Turn", 820, 110, 16, LIGHTGRAY);

    if (const OwnInfantry* sel =
            const_cast<Game&>(g).infantryById(g.selectedUnitId)) {
        DrawText(sel->name.c_str(), 820, 160, 22, YELLOW);
        DrawText(TextFormat("HP %d/%d", sel->hp, sel->maxHp), 820, 188, 18, RAYWHITE);
        DrawText(TextFormat("ATK %d  RNG %d", sel->attackPower, sel->attackRange),
                 820, 210, 18, RAYWHITE);
    }
}