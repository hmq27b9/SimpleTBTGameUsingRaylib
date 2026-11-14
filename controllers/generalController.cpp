//controllers/generalController.cpp
#include "controllers/generalController.hpp"
#include <cstdlib>
#include <queue>
#include <vector>
#include <algorithm>

// ---------- terrain code mapping ----------

Terrain generalController::terrainFromCode(char code) {
    switch (code) {
        case 'R': return Terrain::Road;
        case 'W': return Terrain::Wood;
        case 'H': return Terrain::Hill;
        case '^': return Terrain::Mountain;
        case 'S': return Terrain::Swamp;
        case '~': return Terrain::River;
        default:  return Terrain::Plain;
    }
}

// ---------- input: mouse and keyboard share handleTileClick ----------

void generalController::onMouseClick(int sx, int sy, int ts, int ox, int oy) {
    handleTileClick((sx - ox) / ts, (sy - oy) / ts);
}

void generalController::onTileActivated(int gx, int gy) {
    if (phase_ != Phase::PlayerTurn) return;
    if (!game_.map().inBounds(gx, gy)) return;
    handleTileClick(gx, gy);
}

void generalController::deselect() {
    game_.selectedUnitId = -1;
}

void generalController::selectNextReadyUnit() {
    if (phase_ != Phase::PlayerTurn) return;
    auto& units = game_.ownInfantry();
    if (units.empty()) return;

    // Build the scan-order index list of ready units.
    std::vector<int> ready;
    for (size_t i = 0; i < units.size(); ++i)
        if (units[i].isAlive() && !units[i].hasActed) ready.push_back((int)i);
    if (ready.empty()) { game_.selectedUnitId = -1; return; }

    // Find where the current selection sits in that list.
    int currentPos = -1;
    for (size_t i = 0; i < ready.size(); ++i)
        if (units[ready[i]].id == game_.selectedUnitId) { currentPos = (int)i; break; }

    int nextPos = (currentPos + 1) % (int)ready.size();
    game_.selectedUnitId = units[ready[nextPos]].id;
}

// ---------- pathing: 4-dir Dijkstra weighted by moveCost ----------

std::vector<std::pair<int,int>>
generalController::reachableTiles(int sx, int sy, int moveRange) const {
    std::vector<std::pair<int,int>> out;
    const Map& m = game_.map();
    if (!m.inBounds(sx, sy)) return out;

    std::vector<int> cost(m.width() * m.height(), -1);
    auto idx = [&](int x, int y) { return y * m.width() + x; };

    struct Node { int cost, x, y; };
    struct Cmp { bool operator()(const Node& a, const Node& b) const {
        return a.cost > b.cost;
    }};
    std::priority_queue<Node, std::vector<Node>, Cmp> pq;

    cost[idx(sx,sy)] = 0;
    pq.push({0, sx, sy});

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    while (!pq.empty()) {
        Node cur = pq.top(); pq.pop();
        if (cur.cost != cost[idx(cur.x, cur.y)]) continue;

        for (int i = 0; i < 4; ++i) {
            int nx = cur.x + dx[i], ny = cur.y + dy[i];
            if (!m.inBounds(nx, ny)) continue;
            const Tile& t = m.tileAt(nx, ny);
            if (!t.walkable) continue;
            if (game_.tileOccupied(nx, ny)) continue;

            int step  = t.moveCost;
            int total = cur.cost + step;
            if (total > moveRange) continue;

            int& slot = cost[idx(nx, ny)];
            if (slot != -1 && slot <= total) continue;
            slot = total;
            pq.push({total, nx, ny});
        }
    }

    for (int y = 0; y < m.height(); ++y)
        for (int x = 0; x < m.width(); ++x) {
            if (x == sx && y == sy) continue;
            int c = cost[idx(x, y)];
            if (c >= 0 && c <= moveRange) out.emplace_back(x, y);
        }
    return out;
}

bool generalController::isReachable(int sx, int sy, int moveRange,
                                    int tx, int ty) const {
    if (sx == tx && sy == ty) return false;
    auto tiles = reachableTiles(sx, sy, moveRange);
    for (auto& p : tiles) if (p.first == tx && p.second == ty) return true;
    return false;
}

bool generalController::inMoveRange(int gx, int gy, int moveRange,
                                    int tx, int ty) const {
    return isReachable(gx, gy, moveRange, tx, ty);
}

// ---------- combat: single source of truth ----------

int generalController::previewDamage(int atkPower, int atkHp, int atkMaxHp,
                                     int atkX, int atkY,
                                     int defHp, int defMaxHp,
                                     int defX, int defY) const {
    const Tile& atkT = game_.map().tileAt(atkX, atkY);
    const Tile& defT = game_.map().tileAt(defX, defY);

    int raw = Game::damageFrom(atkPower, atkHp, atkMaxHp);
    raw = raw * atkT.attackMod / 100;
    int actual = Game::damageTakenBy(raw, defHp, defMaxHp);
    actual = actual * defT.defenseMod / 100;
    return std::max(1, actual);
}

void generalController::strike(UnitOwnInfantryLine& attacker,
                               UnitEnemyInfantryLine& defender) {
    int dmg = previewDamage(attacker.attackPower, attacker.hp, attacker.maxHp,
                            attacker.gridX, attacker.gridY,
                            defender.hp, defender.maxHp,
                            defender.gridX, defender.gridY);
    defender.hp -= dmg;
}

void generalController::strike(UnitEnemyInfantryLine& attacker,
                               UnitOwnInfantryLine& defender) {
    int dmg = previewDamage(attacker.attackPower, attacker.hp, attacker.maxHp,
                            attacker.gridX, attacker.gridY,
                            defender.hp, defender.maxHp,
                            defender.gridX, defender.gridY);
    defender.hp -= dmg;
}

// ---------- core turn logic ----------

void generalController::handleTileClick(int gx, int gy) {
    if (phase_ != Phase::PlayerTurn) return;
    if (!game_.map().inBounds(gx, gy)) return;

    if (game_.selectedUnitId == -1) {
        for (auto& u : game_.ownInfantry())
            if (u.isAlive() && u.gridX == gx && u.gridY == gy && !u.hasActed) {
                game_.selectedUnitId = u.id; return;
            }
        return;
    }

    UnitOwnInfantryLine* sel = game_.ownById(game_.selectedUnitId);
    if (!sel) { game_.selectedUnitId = -1; return; }

    for (auto& e : game_.enemyInfantry())
        if (e.isAlive() && e.gridX == gx && e.gridY == gy) { tryAttack(gx, gy); return; }

    for (auto& u : game_.ownInfantry())
        if (u.isAlive() && u.gridX == gx && u.gridY == gy) {
            game_.selectedUnitId = u.id; return;
        }

    tryMove(gx, gy);
}

void generalController::tryMove(int gx, int gy) {
    UnitOwnInfantryLine* sel = game_.ownById(game_.selectedUnitId);
    if (!sel || sel->hasMoved) return;
    if (!inMoveRange(sel->gridX, sel->gridY, sel->moveRange, gx, gy)) return;
    sel->gridX = gx; sel->gridY = gy;
    sel->hasMoved = true;
}

void generalController::tryAttack(int gx, int gy) {
    UnitOwnInfantryLine* sel = game_.ownById(game_.selectedUnitId);
    if (!sel) return;
    UnitEnemyInfantryLine* target = nullptr;
    for (auto& e : game_.enemyInfantry())
        if (e.isAlive() && e.gridX == gx && e.gridY == gy) { target = &e; break; }
    if (!target) return;
    if (std::abs(gx - sel->gridX) + std::abs(gy - sel->gridY) > sel->attackRange) return;

    strike(*sel, *target);
    sel->hasActed = true;

    if (target->isAlive()) {
        int d = std::abs(sel->gridX - target->gridX) +
                std::abs(sel->gridY - target->gridY);
        if (d <= target->attackRange) strike(*target, *sel);
    }
    game_.selectedUnitId = -1;
}

// ---------- enemy turn ----------

bool generalController::tryEnemyStep(UnitEnemyInfantryLine& e) {
    const UnitOwnInfantryLine* nearest = nullptr;
    int bestDist = 9999;
    for (const auto& p : game_.ownInfantry()) {
        if (!p.isAlive()) continue;
        int d = std::abs(p.gridX - e.gridX) + std::abs(p.gridY - e.gridY);
        if (d < bestDist) { bestDist = d; nearest = &p; }
    }
    if (!nearest || bestDist == 1) return false;

    int dx = (nearest->gridX > e.gridX) - (nearest->gridX < e.gridX);
    int dy = (nearest->gridY > e.gridY) - (nearest->gridY < e.gridY);

    struct Step { int x, y; };
    std::vector<Step> candidates;
    if (std::abs(nearest->gridX - e.gridX) >= std::abs(nearest->gridY - e.gridY)) {
        if (dx) candidates.push_back({ e.gridX + dx, e.gridY });
        if (dy) candidates.push_back({ e.gridX, e.gridY + dy });
    } else {
        if (dy) candidates.push_back({ e.gridX, e.gridY + dy });
        if (dx) candidates.push_back({ e.gridX + dx, e.gridY });
    }
    for (auto& c : candidates) {
        if (!game_.map().inBounds(c.x, c.y)) continue;
        if (!game_.map().tileAt(c.x, c.y).walkable) continue;
        if (game_.tileOccupied(c.x, c.y)) continue;
        e.gridX = c.x; e.gridY = c.y; return true;
    }
    return false;
}

void generalController::runEnemyTurn() {
    for (auto& e : game_.enemyInfantry()) {
        if (!e.isAlive()) continue;
        for (int step = 0; step < e.moveRange; ++step)
            if (!tryEnemyStep(e)) break;

        static const int dx[4] = {1, -1, 0, 0};
        static const int dy[4] = {0, 0, 1, -1};
        for (int i = 0; i < 4; ++i) {
            UnitOwnInfantryLine* victim = nullptr;
            for (auto& p : game_.ownInfantry())
                if (p.isAlive() &&
                    p.gridX == e.gridX + dx[i] &&
                    p.gridY == e.gridY + dy[i]) { victim = &p; break; }
            if (!victim) continue;
            strike(e, *victim);
            if (victim->isAlive()) {
                int d = std::abs(e.gridX - victim->gridX) +
                        std::abs(e.gridY - victim->gridY);
                if (d <= victim->attackRange) strike(*victim, e);
            }
            break;
        }
    }
    game_.selectedUnitId = -1;
}

void generalController::onEndTurnPressed() {
    if (phase_ != Phase::PlayerTurn) return;
    for (auto& u : game_.ownInfantry())   { u.hasMoved = false; u.hasActed = false; }
    for (auto& e : game_.enemyInfantry()) { e.hasMoved = false; e.hasActed = false; }
    phase_ = Phase::EnemyTurn;
    runEnemyTurn();
    phase_ = Phase::PlayerTurn;
    ++turnNumber_;
    if (game_.isGameOver()) phase_ = Phase::BattleOver;
}