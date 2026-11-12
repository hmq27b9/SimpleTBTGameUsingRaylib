#pragma once
#include "model/GameModel.hpp"

class BattleController {
public:
    explicit BattleController(GameModel& m) : model_(m) {}
    void onMouseClick(int sx, int sy, int ts, int ox, int oy);
    void onEndTurnPressed();
private:
    void handleTileClick(int gx, int gy);
    void tryMove(int gx, int gy);
    void tryAttack(int gx, int gy);
    bool inMoveRange(const Unit& u, int gx, int gy) const;
    GameModel& model_;
};