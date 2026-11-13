#include "views/components/hudView.hpp"

void hudView::draw(const Game& g, const generalController& ctrl) const {
    DrawText(g.mapName().c_str(), 820, 20, 22, GOLD);

    const char* phaseTxt =
        ctrl.phase() == Phase::PlayerTurn ? "PLAYER TURN" :
        ctrl.phase() == Phase::EnemyTurn  ? "ENEMY TURN"  : "BATTLE OVER";
    DrawText(phaseTxt, 820, 50, 22, RAYWHITE);
    DrawText(TextFormat("Turn %d", ctrl.turnNumber()), 820, 78, 18, LIGHTGRAY);
    DrawText("[SPACE] End Turn", 820, 110, 16, LIGHTGRAY);

    if (g.selectedUnitId != -1) {
        if (const OwnInfantry* u = const_cast<Game&>(g).ownById(g.selectedUnitId)) {
            DrawText(u->name.c_str(), 820, 160, 22, YELLOW);
            DrawText(TextFormat("HP %d/%d", u->hp, u->maxHp), 820, 188, 18, RAYWHITE);
            DrawText(TextFormat("ATK %d  RNG %d", u->attackPower, u->attackRange),
                     820, 210, 18, RAYWHITE);
        }
    }
}