// @MrDragonMister
#pragma once
#include "game.h"
#include "math.h"

// invoke with display::function(hal, ...)
namespace display {
    // remember to .render() after using these functions
    void drawLine(const HAL* hal, int x0, int y0, int x1, int y1, uint16_t color) {
        // Bresenham's line algorithm
        // approximates f(x) = mx + b
        int dx = abs(x1 - x0);
        int sx = x0 < x1 ? 1 : -1;
        int dy = -abs(y1 - y0);
        int sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;

        while (true) {
            hal->drawPixel(x0, y0, color);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) {
                err += dy;
                x0 += sx;
            }
            if (e2 <= dx) {
                err += dx;
                y0 += sy;
            }
        }
    }

    void drawRect(const HAL* hal, int x, int y, int w, int h, uint16_t color) {
        drawLine(hal, x, y, x + w - 1, y, color);
        drawLine(hal, x + w - 1, y, x + w - 1, y + h - 1, color);
        drawLine(hal, x + w - 1, y + h - 1, x, y + h - 1, color);
        drawLine(hal, x, y + h - 1, x, y, color);
    }
}