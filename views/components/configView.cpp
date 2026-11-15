//views/components/configView.cpp
#include "views/components/configView.hpp"
#include "views/components/screenScaler.hpp"

void configView::draw(int previewIndex, bool previewFullscreen,
                      int baseW, int baseH) const {
    const auto& presets = resolutionPresets();
    int idx = previewIndex;
    if (idx < 0) idx = 0;
    if (idx >= (int)presets.size()) idx = (int)presets.size() - 1;
    const ResolutionPreset& p = presets[idx];

    int sw = baseW;
    int sh = baseH;

    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.65f));

    int panelW = 520, panelH = 300;
    int px = (sw - panelW) / 2;
    int py = (sh - panelH) / 2;

    DrawRectangle(px, py, panelW, panelH, { 30, 30, 40, 255 });
    DrawRectangleLines(px, py, panelW, panelH, GOLD);

    DrawTextHi("SETTINGS", px + 24, py + 20, 26, GOLD);

    int y = py + 70;
    DrawTextHi("Resolution", px + 24, y, 18, RAYWHITE);
    DrawTextHi(TextFormat("< %d x %d >", p.w, p.h), px + 240, y, 18, YELLOW);
    DrawTextHi("[LEFT/RIGHT] cycle", px + 24, y + 22, 13, DARKGRAY);
    y += 60;

    DrawTextHi("Display mode", px + 24, y, 18, RAYWHITE);
    DrawTextHi(previewFullscreen ? "Fullscreen" : "Windowed",
               px + 240, y, 18, previewFullscreen ? ORANGE : SKYBLUE);
    DrawTextHi("[F] toggle", px + 24, y + 22, 13, DARKGRAY);
    DrawTextHi("[F11] Fullscreen (anytime)", px + 240, y + 22, 13, DARKGRAY);
    y += 60;

    const char* confirmLabel = previewFullscreen
        ? "[ENTER] Go fullscreen & close"
        : "[ENTER] Apply & close";
    DrawTextHi(confirmLabel, px + 24, y, 15, GREEN);
    DrawTextHi("[ESC] Cancel", px + 24, y + 22, 15, LIGHTGRAY);
}