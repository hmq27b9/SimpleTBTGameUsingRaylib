//views/components/screenScaler.hpp
#pragma once
#include "raylib.h"

class screenScaler {
public:
    static constexpr int SS = 2;   // supersample factor for text/shapes

    screenScaler(int baseW, int baseH);
    ~screenScaler();

    screenScaler(const screenScaler&)            = delete;
    screenScaler& operator=(const screenScaler&) = delete;

    void beginVirtual();
    void endVirtual();

    int baseWidth()  const { return baseW_; }
    int baseHeight() const { return baseH_; }

private:
    int baseW_, baseH_;
    RenderTexture2D target_;
};

// Draw text at a LOGICAL font size and LOGICAL position, but rasterized at
// SS times the density so it stays sharp when the frame is scaled up.
void DrawTextHi(const char* text, int x, int y, int sz, Color c);
void DrawTextHi(const char* text, float x, float y, float sz, Color c);

Vector2 realToVirtual(Vector2 realPos, int baseW, int baseH);