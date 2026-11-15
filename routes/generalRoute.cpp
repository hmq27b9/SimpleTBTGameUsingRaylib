//routes/generalRoute.cpp
#include "routes/generalRoute.hpp"
#include "raylib.h"
#include <algorithm>

generalRoute::generalRoute(const MapData& data) {
    game_       = std::make_unique<Game>(data);
    controller_ = std::make_unique<generalController>(*game_);
    mapView_    = std::make_unique<mapView>(56, 40, 60);       // virtual px
    unitView_   = std::make_unique<unitView>();
    hudView_    = std::make_unique<hudView>();
    configView_ = std::make_unique<configView>();
    scaler_     = std::make_unique<screenScaler>(1280, 720);   // was 1024, 720
    ensureCursorInBounds();
}

generalRoute::~generalRoute() = default;

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
    // Convert the real-window mouse position into virtual coordinates so
    // hit-testing still lands on the right tile when the window is scaled.
    Vector2 real = GetMousePosition();
    Vector2 v    = realToVirtual(real, scaler_->baseWidth(), scaler_->baseHeight());

    int ts = mapView_->tileSize();
    int ox = mapView_->originX();
    int oy = mapView_->originY();
    int gx = (int)((v.x - ox) / ts);
    int gy = (int)((v.y - oy) / ts);
    if (game_->map().inBounds(gx, gy)) {
        cursorGx_ = gx;
        cursorGy_ = gy;
    }
    // Off-board: leave the cursor where it is so keyboard users don't lose
    // their place just because they nudged the mouse.
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

    // Settings + fullscreen shortcuts.
    if (IsKeyPressed(KEY_O) || IsKeyPressed(KEY_F1)) openSettings();
    if (IsKeyPressed(KEY_F11))                        toggleFullscreen();
}

// ---- settings ----

void generalRoute::openSettings() {
    if (scene_ != Scene::Battle) return;
    settingsPreview_ = settings_;
    scene_ = Scene::Settings;
}

void generalRoute::closeSettings(bool apply) {
    if (scene_ != Scene::Settings) return;
    if (apply) {
        settings_ = settingsPreview_;
        applySettings(settings_.presetIndex, settings_.fullscreen);
    }
    // On cancel we do nothing — the window was never mutated while the menu
    // was open (F only edits the preview), so there is nothing to revert.
    scene_ = Scene::Battle;
}

void generalRoute::toggleFullscreen() {
    ToggleFullscreen();
    settings_.fullscreen        = IsWindowFullscreen();
    settingsPreview_.fullscreen = settings_.fullscreen;
}

void generalRoute::applySettings(int presetIndex, bool fullscreen) {
    const auto& presets = resolutionPresets();
    if (presetIndex < 0) presetIndex = 0;
    if (presetIndex >= (int)presets.size()) presetIndex = (int)presets.size() - 1;
    const ResolutionPreset& p = presets[presetIndex];

    if (fullscreen) {
        // Size the windowed backbuffer first so exiting fullscreen later
        // restores this resolution; then enter true fullscreen.
        if (IsWindowFullscreen()) ToggleFullscreen();
        SetWindowSize(p.w, p.h);
        if (!IsWindowFullscreen()) ToggleFullscreen();
    } else {
        if (IsWindowFullscreen()) ToggleFullscreen();
        SetWindowSize(p.w, p.h);
    }
}

void generalRoute::handleSettingsInput() {
    const auto& presets = resolutionPresets();
    int n = (int)presets.size();

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
        settingsPreview_.presetIndex = (settingsPreview_.presetIndex - 1 + n) % n;
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        settingsPreview_.presetIndex = (settingsPreview_.presetIndex + 1) % n;

    if (IsKeyPressed(KEY_F))
        settingsPreview_.fullscreen = !settingsPreview_.fullscreen;

    if (IsKeyPressed(KEY_F11)) toggleFullscreen();

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) closeSettings(true);
    if (IsKeyPressed(KEY_ESCAPE))                           closeSettings(false);
}

// ---- main loop hooks ----

void generalRoute::update() {
    if (scene_ == Scene::Settings) {
        handleSettingsInput();
        return;
    }
    if (scene_ != Scene::Battle) return;

    // Mouse hover updates the cursor when the mouse is over a tile; keyboard
    // then nudges from wherever the cursor currently sits.
    trackMouseCursor();
    handleKeyboardInput();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // Convert the real mouse position to virtual space before hit-testing.
        Vector2 real = GetMousePosition();
        Vector2 v    = realToVirtual(real, scaler_->baseWidth(), scaler_->baseHeight());
        controller_->onMouseClick((int)v.x, (int)v.y,
                                  mapView_->tileSize(),
                                  mapView_->originX(),
                                  mapView_->originY());
    }

    if (controller_->phase() == Phase::BattleOver || game_->isGameOver())
        scene_ = Scene::GameOver;
}

void generalRoute::draw() {
    // Render everything at the fixed virtual resolution; screenScaler blits
    // the result to the real window with letterboxing. This is what makes
    // the map, units, HUD and settings overlay all scale uniformly.
    scaler_->beginVirtual();

    if (scene_ == Scene::Battle || scene_ == Scene::Settings) {
        mapView_->draw(*game_, cursorGx_, cursorGy_);
        unitView_->draw(*game_, *mapView_, *controller_);
        hudView_->draw(*game_, *controller_);
        if (cursorGx_ >= 0 && scene_ == Scene::Battle)
            hudView_->drawHoverPreview(*game_, *controller_, cursorGx_, cursorGy_);

        if (scene_ == Scene::Settings)
            configView_->draw(settingsPreview_.presetIndex,
                              settingsPreview_.fullscreen,
                              scaler_->baseWidth(),
                              scaler_->baseHeight());
    } else {
        DrawText("GAME OVER", 340, 280, 60, RED);
        DrawText("Press ESC to quit", 380, 360, 20, LIGHTGRAY);
    }

    scaler_->endVirtual();
}