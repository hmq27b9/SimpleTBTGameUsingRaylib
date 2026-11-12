#pragma once
#include <memory>
#include "model/GameModel.hpp"
#include "model/MapData.hpp"
#include "controller/BattleController.hpp"
#include "view/MapView.hpp"
#include "view/UnitView.hpp"
#include "view/HUDView.hpp"

enum class Scene { Battle, GameOver };

class Router {
public:
    explicit Router(const MapData& initialMap);
    void update();
    void draw();
private:
    Scene scene_ = Scene::Battle;
    std::unique_ptr<GameModel>        model_;
    std::unique_ptr<BattleController> controller_;
    std::unique_ptr<MapView>          mapView_;
    std::unique_ptr<UnitView>         unitView_;
    std::unique_ptr<HUDView>          hudView_;
};