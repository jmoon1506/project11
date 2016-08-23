#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <SDL2/SDL.h>
#include <cairo/cairo.h>

class Primitives {
 public:
    static int DrawEllipse(SDL_Renderer* renderer, int x, int y, int rx, int ry, SDL_Color color);
    static int DrawRoundedRectangle(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int r, SDL_Color color);
    static int DrawRectangle(SDL_Renderer * renderer, int x1, int y1, int x2, int y2, SDL_Color color);
    static void DrawRoundedRectangleCairo(cairo_t* cr, double x, double y, double w, double h, double cornerRadius, SDL_Color color);

 private:
    static int PixelRGBA(SDL_Renderer* renderer, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static int PixelRGBAWeight(SDL_Renderer* renderer, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint32_t weight);
    static int HLine(SDL_Renderer * renderer, int x1, int x2, int y);
};

#endif