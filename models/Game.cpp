#include "models/Game.hpp"
#include "controllers/generalController.hpp"   // MapData

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
            OwnInfantry u;
            u.id = id++; u.name = s.name;
            u.gridX = s.x; u.gridY = s.y;
            u.hp = u.maxHp = s.hp;
            u.attackPower = s.atk;
            u.moveRange = s.moveRange;
            u.attackRange = s.attackRange;
            ownInfantry_.push_back(u);
        } else {
            EnemyInfantry e;
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

OwnInfantry* Game::ownById(int id) {
    for (auto& u : ownInfantry_) if (u.id == id) return &u;
    return nullptr;
}
EnemyInfantry* Game::enemyById(int id) {
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

bool Game::isGameOver() const {
    bool p = false, e = false;
    for (const auto& u : ownInfantry_)   if (u.isAlive()) { p = true; break; }
    for (const auto& e2 : enemyInfantry_) if (e2.isAlive()) { e = true; break; }
    return !p || !e;
}