//views/components/configView.hpp
#pragma once
#include "raylib.h"
#include <vector>

struct ResolutionPreset { int w, h; };

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

struct ScreenSettings {
    int  presetIndex = 0;
    bool fullscreen  = false;
};

// Main settings menu rows.
enum class SettingsItem {
    Resolution = 0,
    Fullscreen = 1,
    EndTurn    = 2,
    Quit       = 3,
    Count      = 4
};

class configView {
public:
    void draw(int selectedItem,
              int previewIndex, bool previewFullscreen,
              int baseW, int baseH) const;

    void drawResolutionSubmenu(int subIndex,
                               int previewIndex, bool previewFullscreen,
                               int baseW, int baseH) const;
};