//main.cpp
#include "raylib.h"
#include "routes/generalRoute.hpp"
#include "views/mapPrologue/mapPrologue.hpp"

int main() {
    InitWindow(1024, 720, "SimpleTBTGameUsingRaylib");
    SetTargetFPS(60);

    MapData firstMission = mapPrologue::makeMapData();
    generalRoute route(firstMission);

    while (!WindowShouldClose()) {
        route.update();
        route.draw();
    }
    CloseWindow();
    return 0;
}