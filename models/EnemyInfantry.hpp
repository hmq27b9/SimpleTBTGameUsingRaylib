#pragma once
#include <string>

struct EnemyInfantry {
    int id = 0;
    std::string name;
    int gridX = 0, gridY = 0;
    int hp = 10, maxHp = 10;
    int moveRange = 2;
    int attackRange = 1;
    int attackPower = 3;
    bool hasMoved = false;
    bool hasActed = false;

    bool isAlive() const { return hp > 0; }
};