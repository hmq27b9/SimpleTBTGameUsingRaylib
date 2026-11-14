//routes/generalRoute.cpp
#include "routes/generalRoute.hpp"
#include "raylib.h"

generalRoute::generalRoute(const MapData& data) {
    game_       = std::make_unique<Game>(data);
    controller_ = std::make_unique<generalController>(*game_);
    mapView_    = std::make_unique<mapView>(56, 40, 60);
    unitView_   = std::make_unique<unitView>();
    hudView_    = std::make_unique<hudView>();
    ensureCursorInBounds();
}

void generalRoute::ensureCursorInBounds() {
    const Map& m = game_->map();
    if (!m.inBounds(cursorGx_, cursorGy_)) {
        // Start on the first living player unit so the cursor is meaningful.
        for (const auto& u : game_->ownInfantry())
            if (u.isAlive()) { cursorGx_ = u.gridX; cursorGy_ = u.gridY; return; }
        // Fallback: center of the map.
        cursorGx_ = m.width()  / 2;
        cursorGy_ = m.height() / 2;
    }
}

void generalRoute::moveCursor(int dx, int dy) {
    const Map& m = game_->map();
    if (!m.inBounds(cursorGx_, cursorGy_)) { ensureCursorInBounds(); return; }
    int nx = cursorGx_ + dx;
    int ny = cursorGy_ + dy;
    if (m.inBounds(nx, ny)) {
        cursorGx_ = nx;
        cursorGy_ = ny;
    }
}

void generalRoute::trackMouseCursor() {
    Vector2 m = GetMousePosition();
    int ts = mapView_->tileSize();
    int ox = mapView_->originX();
    int oy = mapView_->originY();
    int gx = (int)((m.x - ox) / ts);
    int gy = (int)((m.y - oy) / ts);
    if (game_->map().inBounds(gx, gy)) {
        cursorGx_ = gx;
        cursorGy_ = gy;
    }
    // Off-board: leave the cursor where it is so keyboard users don't lose
    // their place just because they nudged the mouse. Off-board clicks are
    // ignored by onMouseClick (bounds-checked in handleTileClick).
}

void generalRoute::handleKeyboardInput() {
    if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) moveCursor(-1,  0);
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) moveCursor( 1,  0);
    if (IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W)) moveCursor( 0, -1);
    if (IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S)) moveCursor( 0,  1);

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (cursorGx_ >= 0 && cursorGy_ >= 0)
            controller_->onTileActivated(cursorGx_, cursorGy_);
    }
    if (IsKeyPressed(KEY_ESCAPE))  controller_->deselect();
    if (IsKeyPressed(KEY_E))       controller_->onEndTurnPressed();
    if (IsKeyPressed(KEY_TAB))     controller_->selectNextReadyUnit();
}

void generalRoute::update() {
    if (scene_ != Scene::Battle) return;

    // Mouse hover updates the cursor when the mouse is over a tile; keyboard
    // then nudges from wherever the cursor currently sits. Whichever input
    // fired most recently effectively owns the cursor position.
    trackMouseCursor();
    handleKeyboardInput();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        controller_->onMouseClick((int)m.x, (int)m.y,
                                  mapView_->tileSize(),
                                  mapView_->originX(),
                                  mapView_->originY());
    }

    if (controller_->phase() == Phase::BattleOver || game_->isGameOver())
        scene_ = Scene::GameOver;
}

void generalRoute::draw() {
    BeginDrawing();
    ClearBackground({ 30, 30, 40, 255 });

    if (scene_ == Scene::Battle) {
        mapView_->draw(*game_, cursorGx_, cursorGy_);
        unitView_->draw(*game_, *mapView_, *controller_);
        hudView_->draw(*game_, *controller_);
        if (cursorGx_ >= 0)
            hudView_->drawHoverPreview(*game_, *controller_, cursorGx_, cursorGy_);
    } else {
        DrawText("GAME OVER", 340, 280, 60, RED);
        DrawText("Press ESC to quit", 380, 360, 20, LIGHTGRAY);
    }
    EndDrawing();
}