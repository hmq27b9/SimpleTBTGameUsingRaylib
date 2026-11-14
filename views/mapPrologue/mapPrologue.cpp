//views/mapPrologue/mapPrologue.cpp
#include "views/mapPrologue/mapPrologue.hpp"

MapData mapPrologue::makeMapData() {
    MapData d;
    d.id          = "prologue";
    d.displayName = "Prologue";
    d.width  = 12;
    d.height = 10;

    const char* rows[10] = {
        "..^^..WW....",
        ".WW..W..WW..",
        ".WW..~..W...",
        "....S~......",
        "....~~RR....",
        "..RR....SS..",
        ".WW..HH..W..",
        ".WW..HH..W..",
        "....RRRR....",
        "............",
    };
    for (int y = 0; y < d.height; ++y)
        for (int x = 0; x < d.width; ++x)
            if (rows[y][x] != '.')
                d.terrain.push_back({ x, y, rows[y][x] });

    d.units.push_back({ "Aria",  2, 8, true,  10, 3, 2, 1 });
    d.units.push_back({ "Bran",  4, 8, true,  10, 3, 2, 1 });
    d.units.push_back({ "Cato",  6, 8, true,  10, 3, 2, 1 });
    d.units.push_back({ "Goblin", 3, 1, false, 8, 4, 2, 1 });
    d.units.push_back({ "Goblin", 6, 1, false, 8, 4, 2, 1 });
    d.units.push_back({ "Orc",    8, 2, false, 8, 4, 2, 1 });

    return d;
}