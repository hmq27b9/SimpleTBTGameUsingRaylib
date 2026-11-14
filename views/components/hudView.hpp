//views/components/hudView.hpp
#pragma once
#include "models/Game.hpp"
#include "controllers/generalController.hpp"
#include "raylib.h"

class hudView {
public:
    void draw(const Game& g, const generalController& ctrl) const;
    void drawHoverPreview(const Game& g,
                          const generalController& ctrl,
                          int hoverGx, int hoverGy) const;
};