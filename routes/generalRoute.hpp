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

    int cursorGx() const { return cursorGx_; }
    int cursorGy() const { return cursorGy_; }

private:
    void trackMouseCursor();
    void handleKeyboardInput();
    void handleSettingsInput();
    void moveCursor(int dx, int dy);
    void ensureCursorInBounds();

    void openSettings();
    void closeSettings(bool apply);
    void applySettings(int presetIndex, bool fullscreen);
    void toggleFullscreen();

    Scene scene_ = Scene::Battle;
    int cursorGx_ = -1, cursorGy_ = -1;

    std::unique_ptr<Game>              game_;
    std::unique_ptr<generalController> controller_;
    std::unique_ptr<mapView>           mapView_;
    std::unique_ptr<unitView>          unitView_;
    std::unique_ptr<hudView>           hudView_;
    std::unique_ptr<configView>        configView_;
    std::unique_ptr<screenScaler>      scaler_;

    ScreenSettings settings_;
    ScreenSettings settingsPreview_;
};