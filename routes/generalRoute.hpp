//routes/generalRoute.hpp
#pragma once
#include <memory>
#include "models/Game.hpp"
#include "controllers/generalController.hpp"
#include "views/components/mapView.hpp"
#include "views/components/unitView.hpp"
#include "views/components/hudView.hpp"
#include "views/components/configView.hpp"
#include "views/components/screenScaler.hpp"

enum class Scene { Battle, Settings, GameOver };

class generalRoute {
public:
    explicit generalRoute(const MapData& initialMap);
    ~generalRoute();

    void update();
    void draw();

    bool requestQuit() const { return requestQuit_; }

    int cursorGx() const { return cursorGx_; }
    int cursorGy() const { return cursorGy_; }

private:
    void trackMouseCursor();
    void handleKeyboardInput();
    void handleSettingsInput();
    void handleResolutionSubmenuInput();
    void handleGameOverInput();
    void moveCursor(int dx, int dy);
    void ensureCursorInBounds();

    void openSettings();
    void closeSettings(bool apply);
    void applySettings(int presetIndex, bool fullscreen);
    void toggleFullscreen();

    void restartGame();
    Vector2 gameOverButtonCenter(int index) const;
    Rectangle gameOverButtonRect(int index) const;
    int     gameOverHoveredButton() const;

    Scene scene_ = Scene::Battle;
    bool  requestQuit_ = false;
    int cursorGx_ = -1, cursorGy_ = -1;

    MapData originalMap_;

    std::unique_ptr<Game>              game_;
    std::unique_ptr<generalController> controller_;
    std::unique_ptr<mapView>           mapView_;
    std::unique_ptr<unitView>          unitView_;
    std::unique_ptr<hudView>           hudView_;
    std::unique_ptr<configView>        configView_;
    std::unique_ptr<screenScaler>      scaler_;

    ScreenSettings settings_;
    ScreenSettings settingsPreview_;

    int  settingsItem_ = 0;          // SettingsItem cast to int
    bool resolutionSubmenu_ = false; // true while the resolution submenu is open
    int  resolutionSubIndex_ = 0;    // index inside the submenu (0..n-1 = preset, n = fullscreen toggle)
};