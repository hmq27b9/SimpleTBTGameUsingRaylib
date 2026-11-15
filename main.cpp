//main.cpp
#include "raylib.h"
#include "routes/generalRoute.hpp"
#include "views/mapPrologue/mapPrologue.hpp"

int main() {
    InitWindow(1024, 720, "SimpleTBTGameUsingRaylib");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    {
        MapData firstMission = mapPrologue::makeMapData();
        generalRoute route(firstMission);

        while (!WindowShouldClose() && !route.requestQuit()) {
            route.update();
            route.draw();
        }
    }   // destroy route (and its render texture) before CloseWindow

    CloseWindow();
    return 0;
}