#include "model/GameModel.hpp"

GameModel::GameModel(const MapData& d)
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
        Unit u;
        u.id = id++; u.name = s.name;
        u.gridX = s.x; u.gridY = s.y;
        u.isPlayerTeam = s.player;
        u.hp = u.maxHp = s.hp;
        u.attackPower = s.atk;
        u.moveRange = s.moveRange;
        u.attackRange = s.attackRange;
        units_.push_back(u);
    }
}

Unit* GameModel::unitAt(int x, int y) {
    for (auto& u : units_) if (u.isAlive() && u.gridX==x && u.gridY==y) return &u;
    return nullptr;
}
const Unit* GameModel::unitAt(int x, int y) const {
    for (auto& u : units_) if (u.isAlive() && u.gridX==x && u.gridY==y) return &u;
    return nullptr;
}
Unit* GameModel::unitById(int id) {
    for (auto& u : units_) if (u.id==id) return &u;
    return nullptr;
}
bool GameModel::isGameOver() const {
    bool p=false, e=false;
    for (auto& u : units_) {
        if (!u.isAlive()) continue;
        if (u.isPlayerTeam) p = true; else e = true;
    }
    return !p || !e;
}