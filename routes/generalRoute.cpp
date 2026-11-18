//routes/generalRoute.cpp
#include "routes/generalRoute.hpp"
#include "raylib.h"
#include "views/components/screenScaler.hpp"
#include <algorithm>

// -------- Game-over button layout (logical coords, 1280x720 frame) --------

static constexpr int   GOBTN_W       = 220;
static constexpr int   GOBTN_H       = 56;
static constexpr int   GOBTN_GAP     = 40;
static constexpr int   GOBTN_Y       = 460;
static constexpr float GOBTN_HOVER_GROW = 4.0f;

Rectangle generalRoute::gameOverButtonRect(int index) const {
    int total = GOBTN_W * 2 + GOBTN_GAP;
    int startX = (1280 - total) / 2;
    int x = startX + index * (GOBTN_W + GOBTN_GAP);
    return Rectangle{ (float)x, (float)GOBTN_Y,
                      (float)GOBTN_W, (float)GOBTN_H };
}

Vector2 generalRoute::gameOverButtonCenter(int index) const {
    Rectangle r = gameOverButtonRect(index);
    return Vector2{ r.x + r.width / 2, r.y + r.height / 2 };
}

int generalRoute::gameOverHoveredButton() const {
    Vector2 mouse = realToVirtual(GetMousePosition(),
                                  scaler_->baseWidth(),
                                  scaler_->baseHeight());
    for (int i = 0; i < 2; ++i) {
        Rectangle r = gameOverButtonRect(i);
        if (CheckCollisionPointRec(mouse, r)) return i;
    }
    return -1;
}

// -------- Constructor / destructor --------

generalRoute::generalRoute(const MapData& data) : originalMap_(data) {
    game_       = std::make_unique<Game>(data);
    controller_ = std::make_unique<generalController>(*game_);
    mapView_    = std::make_unique<mapView>(56, 40, 60);
    unitView_   = std::make_unique<unitView>();
    hudView_    = std::make_unique<hudView>();
    configView_ = std::make_unique<configView>();
    scaler_     = std::make_unique<screenScaler>(1280, 720);
    ensureCursorInBounds();
}

generalRoute::~generalRoute() = default;

void generalRoute::ensureCursorInBounds() {
    const Map& m = game_->map();
    if (!m.inBounds(cursorGx_, cursorGy_)) {
        for (const auto& u : game_->ownInfantry())
            if (u.isAlive()) { cursorGx_ = u.gridX; cursorGy_ = u.gridY; return; }
        cursorGx_ = m.width()  / 2;
        cursorGy_ = m.height() / 2;
    }
}

void generalRoute::moveCursor(int dx, int dy) {
    const Map& m = game_->map();
    if (!m.inBounds(cursorGx_, cursorGy_)) { ensureCursorInBounds(); return; }
    int nx = cursorGx_ + dx;
    int ny = cursorGy_ + dy;
    if (m.inBounds(nx, ny)) { cursorGx_ = nx; cursorGy_ = ny; }
}

void generalRoute::trackMouseCursor() {
    Vector2 real = GetMousePosition();
    Vector2 v    = realToVirtual(real, scaler_->baseWidth(), scaler_->baseHeight());

    int ts = mapView_->tileSize();
    int ox = mapView_->originX();
    int oy = mapView_->originY();
    int gx = (int)((v.x - ox) / ts);
    int gy = (int)((v.y - oy) / ts);
    if (game_->map().inBounds(gx, gy)) { cursorGx_ = gx; cursorGy_ = gy; }
}

// -------- Restart --------

void generalRoute::restartGame() {
    game_       = std::make_unique<Game>(originalMap_);
    controller_ = std::make_unique<generalController>(*game_);

    scene_        = Scene::Battle;
    cursorGx_     = -1;
    cursorGy_     = -1;
    requestQuit_  = false;
    settingsItem_ = 0;
    resolutionSubmenu_  = false;
    resolutionSubIndex_ = 0;
    ensureCursorInBounds();
}

// -------- Keyboard / settings / game over --------

void generalRoute::handleKeyboardInput() {
    if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) moveCursor(-1,  0);
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) moveCursor( 1,  0);
    if (IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W)) moveCursor( 0, -1);
    if (IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S)) moveCursor( 0,  1);

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        if (cursorGx_ >= 0 && cursorGy_ >= 0)
            controller_->onTileActivated(cursorGx_, cursorGy_);
    }
    if (IsKeyPressed(KEY_E))       controller_->onEndTurnPressed();
    if (IsKeyPressed(KEY_TAB))     controller_->selectNextReadyUnit();
    if (IsKeyPressed(KEY_ESCAPE))  openSettings();
    if (IsKeyPressed(KEY_F11))     toggleFullscreen();
}

void generalRoute::handleSettingsInput() {
    int itemCount = (int)SettingsItem::Count;

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        settingsItem_ = (settingsItem_ - 1 + itemCount) % itemCount;
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        settingsItem_ = (settingsItem_ + 1) % itemCount;

    SettingsItem cur = (SettingsItem)settingsItem_;

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        switch (cur) {
            case SettingsItem::Resolution:
                resolutionSubmenu_  = true;
                resolutionSubIndex_ = 0;
                break;
            case SettingsItem::Fullscreen:
                // Toggle only here — no ←/→ path, so the value can't
                // flip twice in one frame.
                settingsPreview_.fullscreen = !settingsPreview_.fullscreen;
                settings_.fullscreen = settingsPreview_.fullscreen;
                applySettings(settings_.presetIndex, settings_.fullscreen);
                break;
            case SettingsItem::EndTurn:
                scene_ = Scene::Battle;
                controller_->onEndTurnPressed();
                break;
            case SettingsItem::Quit:
                scene_ = Scene::Battle;
                requestQuit_ = true;
                break;
            default: break;
        }
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        settingsPreview_ = settings_;
        scene_ = Scene::Battle;
    }
}

void generalRoute::handleResolutionSubmenuInput() {
    const auto& presets = resolutionPresets();
    int n = (int)presets.size();

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        resolutionSubIndex_ = (resolutionSubIndex_ - 1 + n) % n;
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        resolutionSubIndex_ = (resolutionSubIndex_ + 1) % n;

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        settingsPreview_.presetIndex = resolutionSubIndex_;
        settings_ = settingsPreview_;
        applySettings(settings_.presetIndex, settings_.fullscreen);
        resolutionSubmenu_ = false;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        resolutionSubmenu_ = false;
    }
}

void generalRoute::handleGameOverInput() {
    if (IsKeyPressed(KEY_R)) restartGame();
    if (IsKeyPressed(KEY_ESCAPE)) requestQuit_ = true;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int hovered = gameOverHoveredButton();
        if (hovered == 0) restartGame();
        else if (hovered == 1) requestQuit_ = true;
    }
}

// -------- Settings --------

void generalRoute::openSettings() {
    if (scene_ != Scene::Battle) return;
    settingsPreview_ = settings_;
    settingsItem_ = 0;
    resolutionSubmenu_  = false;
    resolutionSubIndex_ = 0;
    scene_ = Scene::Settings;
}

void generalRoute::closeSettings(bool apply) {
    if (scene_ != Scene::Settings) return;
    if (apply) {
        settings_ = settingsPreview_;
        applySettings(settings_.presetIndex, settings_.fullscreen);
    }
    resolutionSubmenu_ = false;
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
        if (IsWindowFullscreen()) ToggleFullscreen();
        SetWindowSize(p.w, p.h);
        if (!IsWindowFullscreen()) ToggleFullscreen();
    } else {
        if (IsWindowFullscreen()) ToggleFullscreen();
        SetWindowSize(p.w, p.h);
    }
}

// -------- Main loop hooks --------

void generalRoute::update() {
    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) &&
        IsKeyPressed(KEY_Q)) {
        requestQuit_ = true;
    }
    if (requestQuit_) return;

    if (scene_ == Scene::Settings) {
        if (resolutionSubmenu_) handleResolutionSubmenuInput();
        else                    handleSettingsInput();
        return;
    }
    if (scene_ == Scene::GameOver) { handleGameOverInput(); return; }

    trackMouseCursor();
    handleKeyboardInput();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
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

// -------- Draw --------

static void drawButton(Rectangle r, const char* label, bool hovered) {
    Color fill    = hovered ? Color{  80, 140, 200, 255 } : Color{ 50,  60,  80, 255 };
    Color outline = hovered ? SKYBLUE                    : GOLD;
    DrawRectangleRec(r, fill);
    DrawRectangleLinesEx(r, 2.0f, outline);

    int fs = 22;
    int tw = MeasureText(label, fs);
    int tx = (int)(r.x + (r.width  - tw) / 2);
    int ty = (int)(r.y + (r.height - fs) / 2);
    DrawTextHi(label, tx, ty, fs, RAYWHITE);
}

void generalRoute::draw() {
    scaler_->beginVirtual();

    if (scene_ == Scene::Battle || scene_ == Scene::Settings) {
        mapView_->draw(*game_, cursorGx_, cursorGy_);
        unitView_->draw(*game_, *mapView_, *controller_);
        hudView_->draw(*game_, *controller_);
        if (cursorGx_ >= 0 && scene_ == Scene::Battle)
            hudView_->drawHoverPreview(*game_, *controller_, cursorGx_, cursorGy_);

        if (scene_ == Scene::Settings) {
            if (resolutionSubmenu_)
                configView_->drawResolutionSubmenu(
                    resolutionSubIndex_,
                    settingsPreview_.presetIndex,
                    settingsPreview_.fullscreen,
                    scaler_->baseWidth(), scaler_->baseHeight());
            else
                configView_->draw(settingsItem_,
                                  settingsPreview_.presetIndex,
                                  settingsPreview_.fullscreen,
                                  scaler_->baseWidth(), scaler_->baseHeight());
        }
    } else {
        DrawTextHi("GAME OVER", 430, 220, 60, RED);
        DrawTextHi("Press R to restart, ESC to quit, or click below",
                   300, 320, 18, LIGHTGRAY);

        int hovered = gameOverHoveredButton();

        Rectangle r0 = gameOverButtonRect(0);
        if (hovered == 0) {
            r0.x      -= GOBTN_HOVER_GROW;
            r0.y      -= GOBTN_HOVER_GROW;
            r0.width  += GOBTN_HOVER_GROW * 2;
            r0.height += GOBTN_HOVER_GROW * 2;
        }
        drawButton(r0, "RESTART", hovered == 0);

        Rectangle r1 = gameOverButtonRect(1);
        if (hovered == 1) {
            r1.x      -= GOBTN_HOVER_GROW;
            r1.y      -= GOBTN_HOVER_GROW;
            r1.width  += GOBTN_HOVER_GROW * 2;
            r1.height += GOBTN_HOVER_GROW * 2;
        }
        drawButton(r1, "QUIT", hovered == 1);
    }

    scaler_->endVirtual();
}