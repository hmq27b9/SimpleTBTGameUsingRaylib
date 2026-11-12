#include "view/HUDView.hpp"
void HUDView::draw(const GameModel& m) const {
    DrawText(m.mapName().c_str(), 820, 20, 22, GOLD);
    const char* p =
        m.turns().phase() == Phase::PlayerTurn ? "PLAYER TURN" :
        m.turns().phase() == Phase::EnemyTurn  ? "ENEMY TURN"  : "BATTLE OVER";
    DrawText(p, 820, 50, 22, RAYWHITE);
    DrawText(TextFormat("Turn %d", m.turns().turnNumber()), 820, 78, 18, LIGHTGRAY);
    DrawText("[SPACE] End Turn", 820, 110, 16, LIGHTGRAY);
    if (Unit* sel = const_cast<GameModel&>(m).unitById(m.selectedUnitId)) {
        DrawText(sel->name.c_str(), 820, 160, 22, YELLOW);
        DrawText(TextFormat("HP %d/%d", sel->hp, sel->maxHp), 820, 188, 18, RAYWHITE);
        DrawText(TextFormat("ATK %d  RNG %d", sel->attackPower, sel->attackRange),
                 820, 210, 18, RAYWHITE);
    }
}