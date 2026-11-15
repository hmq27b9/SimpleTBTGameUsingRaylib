//views/components/screenScaler.hpp
#pragma once
#include "raylib.h"

// Renders everything at a fixed virtual resolution (baseW x baseH) into an
// off-screen texture, then blits it to the real window with aspect-preserving
// letterboxing. All views keep working in virtual coordinates.
class screenScaler {
public:
    screenScaler(int baseW, int baseH);
    ~screenScaler();

    screenScaler(const screenScaler&)            = delete;
    screenScaler& operator=(const screenScaler&) = delete;

    // Call instead of BeginDrawing(). Opens the render-texture pass.
    void beginVirtual();

    // Call instead of EndDrawing(). Closes the render-texture pass, blits it
    // to the window scaled to fit, and calls EndDrawing().
    void endVirtual();

    int baseWidth()  const { return baseW_; }
    int baseHeight() const { return baseH_; }

private:
    int baseW_, baseH_;
    RenderTexture2D target_;
};

// Helper: convert a real-window mouse position into virtual coordinates,
// so hit-testing still works when the window is bigger than the base size.
Vector2 realToVirtual(Vector2 realPos, int baseW, int baseH);