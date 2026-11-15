//views/components/configView.hpp
#pragma once
#include "raylib.h"
#include <vector>

struct ResolutionPreset { int w, h; };

// All resolutions we offer in the menu. Must be >= the HUD area (820px)
// or the HUD will overlap the board — the first preset is the default.
inline const std::vector<ResolutionPreset>& resolutionPresets() {
    static const std::vector<ResolutionPreset> presets = {
        {1024, 720},
        {1280, 720},
        {1280, 800},
        {1366, 768},
        {1600, 900},
        {1920, 1080},
    };
    return presets;
}

// Runtime screen settings, owned by generalRoute.
struct ScreenSettings {
    int  presetIndex = 0;
    bool fullscreen  = false;
};

class configView {
public:
    // Draws the settings overlay.
    //
    // baseW/baseH are the VIRTUAL frame dimensions (the render-texture size),
    // not the real window size. All coordinates in this function are virtual,
    // and the screenScaler blits the result scaled to the real window. Do
    // NOT use GetScreenWidth()/GetScreenHeight() inside here — during
    // BeginTextureMode() raylib reports the *window* size, not the texture
    // size, which is what caused the panel to fall off-screen in fullscreen.
    void draw(int previewIndex, bool previewFullscreen,
              int baseW, int baseH) const;
};