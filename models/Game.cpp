//models/Game.cpp
#include "models/Game.hpp"
#include "controllers/generalController.hpp"   // MapData, terrainFromCode
#include <algorithm>

// Per-terrain defaults: walkable / moveCost / attackMod / defenseMod.
struct TerrainDefaults {
    bool walkable;
    int  moveCost;
    int  attackMod;
    int  defenseMod;
};
static TerrainDefaults defaultsFor(Terrain t) {
    switch (t) {
        case Terrain::Plain:    return { true,  1, 100, 100 };
        case Terrain::Road:     return { true,  1, 100,  90 };
        case Terrain::Wood:     return { true,  2,  90,  75 };
        case Terrain::Hill:     return { true,  2, 120,  90 };
        case Terrain::Mountain: return { false, 3, 130,  60 };
        case Terrain::Swamp:    return { true,  3,  80, 110 };
        case Terrain::River:    return { false, 1, 100, 100 };
    }
    return { true, 1, 100, 100 };
}

Game::Game(const MapData& d)
    : map_(d.width, d.height), mapId_(d.id), mapName_(d.displayName)
{
    // First pass: stamp every tile with its terrain defaults.
    for (int y = 0; y < d.height; ++y)
        for (int x = 0; x < d.width; ++x) {
            Tile& tile = map_.tileAt(x, y);
            TerrainDefaults td = defaultsFor(tile.terrain);
            tile.walkable   = td.walkable;
            tile.moveCost   = td.moveCost;
            tile.attackMod  = td.attackMod;
            tile.defenseMod = td.defenseMod;
        }

    // Second pass: apply terrain spawns from the map data.
    for (const auto& t : d.terrain) {
        if (!map_.inBounds(t.x, t.y)) continue;
        Tile& tile = map_.tileAt(t.x, t.y);
        tile.terrain = generalController::terrainFromCode(t.code);
        TerrainDefaults td = defaultsFor(tile.terrain);
        tile.walkable   = td.walkable;
        tile.moveCost   = td.moveCost;
        tile.attackMod  = td.attackMod;
        tile.defenseMod = td.defenseMod;
    }

    int id = 1;
    for (const auto& s : d.units) {
        if (s.player) {
            UnitOwnInfantryLine u;
            u.id = id++; u.name = s.name;
            u.gridX = s.x; u.gridY = s.y;
            u.hp = u.maxHp = s.hp;
            u.attackPower = s.atk;
            u.moveRange = s.moveRange;
            u.attackRange = s.attackRange;
            ownInfantry_.push_back(u);
        } else {
            UnitEnemyInfantryLine e;
            e.id = id++; e.name = s.name;
            e.gridX = s.x; e.gridY = s.y;
            e.hp = e.maxHp = s.hp;
            e.attackPower = s.atk;
            e.moveRange = s.moveRange;
            e.attackRange = s.attackRange;
            enemyInfantry_.push_back(e);
        }
    }
}

// ---- lookups (must exist for the controller/HUD to link) ----

UnitOwnInfantryLine* Game::ownById(int id) {
    for (auto& u : ownInfantry_) if (u.id == id) return &u;
    return nullptr;
}
UnitEnemyInfantryLine* Game::enemyById(int id) {
    for (auto& e : enemyInfantry_) if (e.id == id) return &e;
    return nullptr;
}

bool Game::tileOccupied(int x, int y) const {
    for (const auto& u : ownInfantry_)
        if (u.isAlive() && u.gridX == x && u.gridY == y) return true;
    for (const auto& e : enemyInfantry_)
        if (e.isAlive() && e.gridX == x && e.gridY == y) return true;
    return false;
}

// ---- combat helpers ----

int Game::damageFrom(int attackPower, int hp, int maxHp) {
    if (maxHp <= 0) return 0;
    int dmg = (attackPower * hp + maxHp / 2) / maxHp;
    return std::max(1, dmg);
}

int Game::damageTakenBy(int incoming, int hp, int maxHp) {
    if (maxHp <= 0) return 0;
    int dmg = (incoming * hp + maxHp / 2) / maxHp;
    return std::max(1, dmg);
}

bool Game::isGameOver() const {
    bool p = false, e = false;
    for (const auto& u : ownInfantry_)    if (u.isAlive()) { p = true; break; }
    for (const auto& e2 : enemyInfantry_) if (e2.isAlive()) { e = true; break; }
    return !p || !e;
}