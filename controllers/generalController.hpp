//controllers/generalController.hpp
#pragma once
#include "models/Game.hpp"
#include <string>
#include <utility>
#include <vector>

struct TerrainSpawn { int x, y; char code; };
struct InfantrySpawn {
    std::string name; int x, y; bool player;
    int hp = 10, atk = 3, moveRange = 2, attackRange = 1;
};
struct MapData {
    std::string id, displayName;
    int width = 0, height = 0;
    std::vector<TerrainSpawn>  terrain;
    std::vector<InfantrySpawn> units;
};

enum class Phase { PlayerTurn, EnemyTurn, BattleOver };

class generalController {
public:
    explicit generalController(Game& g) : game_(g) {}
    Phase phase() const { return phase_; }
    int   turnNumber() const { return turnNumber_; }

    void onMouseClick(int sx, int sy, int tileSize, int originX, int originY);

    // Grid-space equivalent of "click this tile" — used by keyboard activation
    // and reusable by tests. Delegates to the same handleTileClick.
    void onTileActivated(int gx, int gy);

    void onEndTurnPressed();
    void deselect();

    // Advances selection to the next player unit that has not acted, in
    // scan order, wrapping around. No-op if no such unit exists.
    void selectNextReadyUnit();

    static Terrain terrainFromCode(char code);

    int previewDamage(int atkPower, int atkHp, int atkMaxHp,
                      int atkX, int atkY,
                      int defHp, int defMaxHp,
                      int defX, int defY) const;

    std::vector<std::pair<int,int>> reachableTiles(int sx, int sy, int moveRange) const;
    bool isReachable(int sx, int sy, int moveRange, int tx, int ty) const;

private:
    void handleTileClick(int gx, int gy);
    void tryMove(int gx, int gy);
    void tryAttack(int gx, int gy);
    bool inMoveRange(int gridX, int gridY, int moveRange,
                     int tx, int ty) const;
    void runEnemyTurn();
    bool tryEnemyStep(UnitEnemyInfantryLine& e);

    void strike(UnitOwnInfantryLine& attacker, UnitEnemyInfantryLine& defender);
    void strike(UnitEnemyInfantryLine& attacker, UnitOwnInfantryLine& defender);

    Game& game_;
    Phase phase_      = Phase::PlayerTurn;
    int   turnNumber_ = 1;
};