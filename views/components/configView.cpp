//views/components/configView.cpp
#include "views/components/configView.hpp"
#include "views/components/screenScaler.hpp"

static constexpr int PANEL_W = 520;
static constexpr int PANEL_H = 340;

static void drawPanelFrame(int baseW, int baseH, int& px, int& py, int w, int h) {
    DrawRectangle(0, 0, baseW, baseH, Fade(BLACK, 0.65f));
    px = (baseW - w) / 2;
    py = (baseH - h) / 2;
    DrawRectangle(px, py, w, h, { 30, 30, 40, 255 });
    DrawRectangleLines(px, py, w, h, GOLD);
}

void configView::draw(int selectedItem,
                      int previewIndex, bool previewFullscreen,
                      int baseW, int baseH) const {
    const auto& presets = resolutionPresets();
    int idx = previewIndex;
    if (idx < 0) idx = 0;
    if (idx >= (int)presets.size()) idx = (int)presets.size() - 1;
    const ResolutionPreset& p = presets[idx];

    int px, py;
    drawPanelFrame(baseW, baseH, px, py, PANEL_W, PANEL_H);

    DrawTextHi("SETTINGS", px + 24, py + 20, 26, GOLD);

    // ---- Row 0: Resolution (opens submenu on confirm) ----
    int y = py + 80;
    bool selRes = (selectedItem == (int)SettingsItem::Resolution);
    if (selRes)
        DrawRectangle(px + 12, y - 4, PANEL_W - 24, 30, Fade(YELLOW, 0.12f));
    DrawTextHi("Resolution", px + 24, y, 18, selRes ? YELLOW : RAYWHITE);
    DrawTextHi(TextFormat("%d x %d", p.w, p.h), px + 260, y, 18, LIGHTGRAY);
    DrawTextHi(">", px + PANEL_W - 36, y, 18, selRes ? YELLOW : DARKGRAY);
    y += 56;

    // ---- Row 1: Fullscreen (ENTER toggles) ----
    bool selFs = (selectedItem == (int)SettingsItem::Fullscreen);
    if (selFs)
        DrawRectangle(px + 12, y - 4, PANEL_W - 24, 30, Fade(YELLOW, 0.12f));
    DrawTextHi("Fullscreen", px + 24, y, 18, selFs ? YELLOW : RAYWHITE);
    DrawTextHi(previewFullscreen ? "ON" : "OFF", px + 260, y, 18,
               previewFullscreen ? ORANGE : SKYBLUE);
    y += 56;

    // ---- Row 2: End Turn ----
    bool selEnd = (selectedItem == (int)SettingsItem::EndTurn);
    if (selEnd)
        DrawRectangle(px + 12, y - 4, PANEL_W - 24, 30, Fade(YELLOW, 0.12f));
    DrawTextHi("End Turn", px + 24, y, 18, selEnd ? YELLOW : RAYWHITE);
    y += 56;

    // ---- Row 3: Quit ----
    bool selQuit = (selectedItem == (int)SettingsItem::Quit);
    if (selQuit)
        DrawRectangle(px + 12, y - 4, PANEL_W - 24, 30, Fade(ORANGE, 0.15f));
    DrawTextHi("Quit Game", px + 24, y, 18, selQuit ? ORANGE : RAYWHITE);
}

void configView::drawResolutionSubmenu(int subIndex,
                                       int previewIndex, bool previewFullscreen,
                                       int baseW, int baseH) const {
    (void)previewFullscreen;   // fullscreen lives on the main menu now
    const auto& presets = resolutionPresets();
    int n = (int)presets.size();

    int px, py;
    drawPanelFrame(baseW, baseH, px, py, PANEL_W, PANEL_H);

    DrawTextHi("RESOLUTION", px + 24, py + 20, 26, GOLD);

    int y = py + 80;
    for (int i = 0; i < n; ++i) {
        bool sel = (i == subIndex);
        if (sel)
            DrawRectangle(px + 12, y - 4, PANEL_W - 24, 26, Fade(YELLOW, 0.12f));
        DrawTextHi(TextFormat("%d x %d", presets[i].w, presets[i].h),
                   px + 40, y, 17, sel ? YELLOW : RAYWHITE);
        if (i == previewIndex)
            DrawTextHi("(current)", px + 260, y, 14, SKYBLUE);
        y += 30;
    }
}