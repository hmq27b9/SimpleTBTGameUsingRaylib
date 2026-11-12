#pragma once
#include "model/MapData.hpp"

// The prologue view owns the prologue map data.
// It builds a MapData that Router/GameModel can consume.
// (Later: also holds prologue-specific draw calls — intro banner, etc.)
class PrologueView {
public:
    static MapData makeMapData();  // called once by main.cpp
};