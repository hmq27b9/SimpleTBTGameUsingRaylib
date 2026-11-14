//views/components/unitView.hpp
#pragma once
#include "models/Game.hpp"
#include "views/components/mapView.hpp"
#include "controllers/generalController.hpp"

class unitView {
public:
    void draw(const Game& g, const mapView& mv,
              const generalController& ctrl) const;
};