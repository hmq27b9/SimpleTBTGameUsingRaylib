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
    void onEndTurnPressed();

    static Terrain terrainFromCode(char code);

    // Damage that would be dealt from attacker tile -> defender tile, after
    // HP-scaling and both terrain modifiers. Same math as strike(), but pure.
    int previewDamage(int atkPower, int atkHp, int atkMaxHp,
                      int atkX, int atkY,
                      int defHp, int defMaxHp,
                      int defX, int defY) const;

    // Every tile reachable from (sx,sy) within moveRange, using tile moveCost.
    // Returned tiles exclude the origin and occupied/walkable-blocked tiles.
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