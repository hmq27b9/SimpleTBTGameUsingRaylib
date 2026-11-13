#pragma once
#include "models/Game.hpp"
#include <string>
#include <vector>

// ---- Map definition types (owned by the controller layer) ----
struct TerrainSpawn { int x, y; char code; };
struct InfantrySpawn {
    std::string name; int x, y; bool player;
    int hp = 10, atk = 3, moveRange = 2, attackRange = 1;
};
struct MapData {
    std::string id;
    std::string displayName;
    int width = 0, height = 0;
    std::vector<TerrainSpawn>  terrain;
    std::vector<InfantrySpawn> units;
};
// -------------------------------------------------------------

enum class Phase { PlayerTurn, EnemyTurn, BattleOver };

class generalController {
public:
    explicit generalController(Game& g) : game_(g) {}

    Phase phase()      const { return phase_; }
    int   turnNumber() const { return turnNumber_; }

    void onMouseClick(int sx, int sy, int tileSize, int originX, int originY);
    void onEndTurnPressed();

private:
    void handleTileClick(int gx, int gy);
    void tryMove(int gx, int gy);
    void tryAttack(int gx, int gy);
    bool inMoveRange(const OwnInfantry& u, int gx, int gy) const;
    void runEnemyTurn();

    Game& game_;
    Phase phase_      = Phase::PlayerTurn;
    int   turnNumber_ = 1;
};