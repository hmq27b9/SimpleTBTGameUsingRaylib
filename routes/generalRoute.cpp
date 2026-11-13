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

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        controller_->onMouseClick((int)m.x, (int)m.y,
                                  mapView_->tileSize(),
                                  mapView_->originX(),
                                  mapView_->originY());
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
    } else {
        DrawText("GAME OVER", 340, 280, 60, RED);
        DrawText("Press ESC to quit", 380, 360, 20, LIGHTGRAY);
    }
    EndDrawing();
}