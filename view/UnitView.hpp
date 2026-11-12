#pragma once
#include "model/GameModel.hpp"
#include "view/MapView.hpp"
class UnitView {
public:
    void draw(const GameModel&, const MapView&) const;
};