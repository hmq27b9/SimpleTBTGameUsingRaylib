//views/components/configView.cpp
#include "views/components/configView.hpp"

void configView::draw(int previewIndex, bool previewFullscreen,
                      int baseW, int baseH) const {
    const auto& presets = resolutionPresets();
    int idx = previewIndex;
    if (idx < 0) idx = 0;
    if (idx >= (int)presets.size()) idx = (int)presets.size() - 1;
    const ResolutionPreset& p = presets[idx];

    // Work strictly in virtual coordinates. Never call GetScreenWidth() here:
    // inside BeginTextureMode() it returns the real window size, which would
    // place the panel outside the 1024x720 render texture in fullscreen.
    int sw = baseW;
    int sh = baseH;

    // Dim the battle underneath so the menu reads as a modal overlay.
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.65f));

    int panelW = 520;
    int panelH = 300;
    int px = (sw - panelW) / 2;
    int py = (sh - panelH) / 2;

    DrawRectangle(px, py, panelW, panelH, { 30, 30, 40, 255 });
    DrawRectangleLines(px, py, panelW, panelH, GOLD);

    DrawText("SETTINGS", px + 24, py + 20, 26, GOLD);

    int y = py + 70;
    DrawText("Resolution", px + 24, y, 18, RAYWHITE);
    DrawText(TextFormat("< %d x %d >", p.w, p.h),
             px + 240, y, 18, YELLOW);
    DrawText("[LEFT/RIGHT] cycle", px + 24, y + 22, 13, DARKGRAY);
    y += 60;

    DrawText("Display mode", px + 24, y, 18, RAYWHITE);
    DrawText(previewFullscreen ? "Fullscreen" : "Windowed",
             px + 240, y, 18, previewFullscreen ? ORANGE : SKYBLUE);
    DrawText("[F] toggle", px + 24, y + 22, 13, DARKGRAY);
    DrawText("[F11] Fullscreen (anytime)", px + 240, y + 22, 13, DARKGRAY);
    y += 60;

    // Name the pending action so it's obvious what ENTER will do.
    const char* confirmLabel = previewFullscreen
        ? "[ENTER] Go fullscreen & close"
        : "[ENTER] Apply & close";
    DrawText(confirmLabel, px + 24, y, 15, GREEN);
    DrawText("[ESC] Cancel", px + 24, y + 22, 15, LIGHTGRAY);
}