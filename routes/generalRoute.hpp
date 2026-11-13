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
private:
    Scene scene_ = Scene::Battle;
    std::unique_ptr<Game>              game_;
    std::unique_ptr<generalController> controller_;
    std::unique_ptr<mapView>           mapView_;
    std::unique_ptr<unitView>          unitView_;
    std::unique_ptr<hudView>           hudView_;
};