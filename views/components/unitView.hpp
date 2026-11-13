#pragma once
#include "models/Game.hpp"
#include "views/components/mapView.hpp"

class unitView {
public:
    void draw(const Game& g, const mapView& mv) const;
};