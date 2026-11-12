#include "router/Router.hpp"
#include "raylib.h"

Router::Router(const MapData& data) {
    model_      = std::make_unique<GameModel>(data);
    controller_ = std::make_unique<BattleController>(*model_);
    mapView_    = std::make_unique<MapView>(56, 40, 60);
    unitView_   = std::make_unique<UnitView>();
    hudView_    = std::make_unique<HUDView>();
}
void Router::update() {
    if (scene_ != Scene::Battle) return;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        controller_->onMouseClick((int)m.x, (int)m.y,
                                  mapView_->tileSize(),
                                  mapView_->originX(),
                                  mapView_->originY());
    }
    if (IsKeyPressed(KEY_SPACE)) controller_->onEndTurnPressed();
    if (model_->isGameOver()) scene_ = Scene::GameOver;
}
void Router::draw() {
    BeginDrawing();
    ClearBackground({ 30, 30, 40, 255 });
    if (scene_ == Scene::Battle) {
        mapView_->draw(*model_);
        unitView_->draw(*model_, *mapView_);
        hudView_->draw(*model_);
    } else {
        DrawText("GAME OVER", 340, 280, 60, RED);
        DrawText("Press ESC to quit", 380, 360, 20, LIGHTGRAY);
    }
    EndDrawing();
}