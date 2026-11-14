//models/Game.cpp
#include "models/Game.hpp"
#include "controllers/generalController.hpp"   // MapData
#include <algorithm>

Game::Game(const MapData& d)
    : map_(d.width, d.height), mapId_(d.id), mapName_(d.displayName)
{
    for (const auto& t : d.terrain) {
        if (!map_.inBounds(t.x, t.y)) continue;
        Tile& tile = map_.tileAt(t.x, t.y);
        switch (t.code) {
            case 'F': tile.terrain = Terrain::Forest;   tile.moveCost = 2; break;
            case '~': tile.terrain = Terrain::Water;    tile.walkable = false; break;
            case '^': tile.terrain = Terrain::Mountain; tile.walkable = false; break;
        }
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

int Game::damageFrom(int attackPower, int hp, int maxHp) {
    if (maxHp <= 0) return 0;
    // Proportional to attacker's remaining HP fraction, rounded, min 1.
    int dmg = (attackPower * hp + maxHp / 2) / maxHp;
    return std::max(1, dmg);
}

int Game::damageTakenBy(int incoming, int hp, int maxHp) {
    if (maxHp <= 0) return 0;
    // Wounded units take proportionally less raw damage; min 1 so hits never heal.
    int dmg = (incoming * hp + maxHp / 2) / maxHp;
    return std::max(1, dmg);
}

bool Game::isGameOver() const {
    bool p = false, e = false;
    for (const auto& u : ownInfantry_)   if (u.isAlive()) { p = true; break; }
    for (const auto& e2 : enemyInfantry_) if (e2.isAlive()) { e = true; break; }
    return !p || !e;
}