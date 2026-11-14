//routes/generalRoute.hpp
#pragma once
#include <memory>
#include "models/Game.hpp"
#include "controllers/generalController.hpp"
#include "views/components/mapView.hpp"
#include "views/components/unitView.hpp"
#include "views/components/hudView.hpp"

enum class Scene { Battle, GameOver };

class generalRoute {
public:
    explicit generalRoute(const MapData& initialMap);
    void update();
    void draw();

    int cursorGx() const { return cursorGx_; }
    int cursorGy() const { return cursorGy_; }

private:
    void trackMouseCursor();
    void handleKeyboardInput();
    void moveCursor(int dx, int dy);
    void ensureCursorInBounds();

    Scene scene_ = Scene::Battle;
    int cursorGx_ = -1, cursorGy_ = -1;
    std::unique_ptr<Game>              game_;
    std::unique_ptr<generalController> controller_;
    std::unique_ptr<mapView>           mapView_;
    std::unique_ptr<unitView>          unitView_;
    std::unique_ptr<hudView>           hudView_;
};