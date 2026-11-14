//routes/generalRoute.cpp
#include "routes/generalRoute.hpp"
#include "raylib.h"

generalRoute::generalRoute(const MapData& data) {
    game_       = std::make_unique<Game>(data);
    controller_ = std::make_unique<generalController>(*game_);
    mapView_    = std::make_unique<mapView>(56, 40, 60);
    unitView_   = std::make_unique<unitView>();
    hudView_    = std::make_unique<hudView>();
}

void generalRoute::update() {
    if (scene_ != Scene::Battle) return;

    // Track hovered tile every frame.
    Vector2 m = GetMousePosition();
    int ts = mapView_->tileSize();
    int ox = mapView_->originX();
    int oy = mapView_->originY();
    hoverGx_ = (int)((m.x - ox) / ts);
    hoverGy_ = (int)((m.y - oy) / ts);
    if (!game_->map().inBounds(hoverGx_, hoverGy_))
        hoverGx_ = hoverGy_ = -1;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        controller_->onMouseClick((int)m.x, (int)m.y, ts, ox, oy);
    }
    if (IsKeyPressed(KEY_SPACE)) controller_->onEndTurnPressed();

    if (controller_->phase() == Phase::BattleOver || game_->isGameOver())
        scene_ = Scene::GameOver;
}

void generalRoute::draw() {
    BeginDrawing();
    ClearBackground({ 30, 30, 40, 255 });

    if (scene_ == Scene::Battle) {
        mapView_->draw(*game_);
        unitView_->draw(*game_, *mapView_);
        hudView_->draw(*game_, *controller_);
        if (hoverGx_ >= 0)
            hudView_->drawHoverPreview(*game_, *controller_, hoverGx_, hoverGy_);
    } else {
        DrawText("GAME OVER", 340, 280, 60, RED);
        DrawText("Press ESC to quit", 380, 360, 20, LIGHTGRAY);
    }
    EndDrawing();
}