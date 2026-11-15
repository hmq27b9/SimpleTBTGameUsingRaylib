//views/components/screenScaler.cpp
#include "views/components/screenScaler.hpp"
#include <algorithm>

screenScaler::screenScaler(int baseW, int baseH) : baseW_(baseW), baseH_(baseH) {
    target_ = LoadRenderTexture(baseW_, baseH_);
    // Nearest-neighbour keeps the pixel-art tiles crisp when scaled up.
    SetTextureFilter(target_.texture, TEXTURE_FILTER_POINT);
}

screenScaler::~screenScaler() {
    UnloadRenderTexture(target_);
}

void screenScaler::beginVirtual() {
    BeginTextureMode(target_);
    ClearBackground({ 30, 30, 40, 255 });
}

void screenScaler::endVirtual() {
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);   // letterbox bars

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // Fit the virtual frame inside the window without distorting aspect.
    float scale = std::min((float)sw / baseW_, (float)sh / baseH_);
    int dw = (int)(baseW_ * scale);
    int dh = (int)(baseH_ * scale);
    int dx = (sw - dw) / 2;
    int dy = (sh - dh) / 2;

    // Render textures are stored flipped in OpenGL — invert source height.
    Rectangle src = { 0, 0, (float)baseW_, (float)-baseH_ };
    Rectangle dst = { (float)dx, (float)dy, (float)dw, (float)dh };
    DrawTexturePro(target_.texture, src, dst, { 0, 0 }, 0.0f, WHITE);

    EndDrawing();
}

Vector2 realToVirtual(Vector2 realPos, int baseW, int baseH) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    if (sw == 0 || sh == 0) return realPos;

    float scale = std::min((float)sw / baseW, (float)sh / baseH);
    int dw = (int)(baseW * scale);
    int dh = (int)(baseH * scale);
    int dx = (sw - dw) / 2;
    int dy = (sh - dh) / 2;

    // Inverse of the blit transform.
    float vx = (realPos.x - dx) / scale;
    float vy = (realPos.y - dy) / scale;
    return { vx, vy };
}