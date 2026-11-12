#include "raylib.h"
#include "router/Router.hpp"
#include "view/prologue/PrologueView.hpp"

int main() {
    InitWindow(1024, 720, "SimpleTBTGameUsingRaylib");
    SetTargetFPS(60);

    MapData firstMission = PrologueView::makeMapData();
    Router router(firstMission);

    while (!WindowShouldClose()) {
        router.update();
        router.draw();
    }
    CloseWindow();
    return 0;
}