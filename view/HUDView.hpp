#pragma once
#include "model/GameModel.hpp"
#include "raylib.h"
class HUDView {
public:
    void draw(const GameModel&) const;
};