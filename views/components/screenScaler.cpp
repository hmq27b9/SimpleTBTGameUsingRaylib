//views/components/screenScaler.cpp
#include "views/components/screenScaler.hpp"
#include "rlgl.h"
#include <algorithm>
#include <cmath>

screenScaler::screenScaler(int baseW, int baseH) : baseW_(baseW), baseH_(baseH) {
    target_ = LoadRenderTexture(baseW_ * SS, baseH_ * SS);
    SetTextureFilter(target_.texture, TEXTURE_FILTER_BILINEAR);
}

screenScaler::~screenScaler() {
    if (IsWindowReady()) UnloadRenderTexture(target_);
}

void screenScaler::beginVirtual() {
    BeginTextureMode(target_);
    ClearBackground({ 30, 30, 40, 255 });

    // Shapes (rectangles/lines) inherit this 2x modelview scale, so they
    // render at SS-times density without any call-site changes.
    rlPushMatrix();
    rlScalef((float)SS, (float)SS, 1.0f);
}

void screenScaler::endVirtual() {
    rlPopMatrix();
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    float scale = std::min((float)sw / baseW_, (float)sh / baseH_);
    int dw = (int)(baseW_ * scale);
    int dh = (int)(baseH_ * scale);
    int dx = (sw - dw) / 2;
    int dy = (sh - dh) / 2;

    Rectangle src = { 0, 0, (float)target_.texture.width, -(float)target_.texture.height };
    Rectangle dst = { (float)dx, (float)dy, (float)dw, (float)dh };
    DrawTexturePro(target_.texture, src, dst, { 0, 0 }, 0.0f, WHITE);

    EndDrawing();
}

void DrawTextHi(const char* text, int x, int y, int sz, Color c) {
    // Bypass the ambient rlScalef so we can rasterize glyphs at SS*size
    // (sharp) instead of blitting size-sized glyphs and stretching them
    // (blurry). Position is multiplied manually to stay in the same visual
    // spot the call site expects.
    rlPushMatrix();
    rlLoadIdentity();
    DrawText(text, x * screenScaler::SS, y * screenScaler::SS,
             sz * screenScaler::SS, c);
    rlPopMatrix();
}

void DrawTextHi(const char* text, float x, float y, float sz, Color c) {
    rlPushMatrix();
    rlLoadIdentity();
    DrawText(text,
             (int)std::round(x * screenScaler::SS),
             (int)std::round(y * screenScaler::SS),
             (int)std::round(sz * screenScaler::SS), c);
    rlPopMatrix();
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

    return { (realPos.x - dx) / scale, (realPos.y - dy) / scale };
}