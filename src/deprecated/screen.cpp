#include <SDL2/SDL.h>
#include "screen.h"

/*
Screen::Screen() {
}
*/

void Screen::Init() {
    
}

void Screen::Resize(SDL_Window* wndw, int w, int h) {

}

void Screen::Fullscreen(SDL_Window* wndw, bool fulscr) {

}

/*
void Screen::InitScreen()
{
    SDL_DisplayMode display;
    SDL_GetCurrentDisplayMode(0, &display);
    screen_w = display.w;
    screen_h = display.h;

    wndw_w = 1024;
    wndw_h = 600;

    scale_x = 1;
    scale_y = 1;

    std::cout << "screen: " << screen_w << "x" << screen_h << " window: " << wndw_w << "x" << wndw_h << std::endl;
}

void Screen::Fullscreen(SDL_Window* window, bool fulscr)
{
    if (fulscr) {
        SDL_DisplayMode mode;
        mode.format = SDL_PIXELFORMAT_ARGB8888;
        SDL_GetCurrentDisplayMode(0, &mode);
        mode.w = screen_w;
        mode.h = screen_h;
        mode.refresh_rate = 0;
        mode.driverdata = 0;
        SDL_SetWindowDisplayMode(window, &mode);

        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_SetWindowSize(window, screen_w, screen_h);

        scale_x = (float)screen_w / (float)wndw_w;
        scale_y = (float)screen_h / (float)wndw_h;

        fullscreen = true;
    }
    else {
        SDL_SetWindowFullscreen(window, 0);
        SDL_SetWindowSize(window, 1024, 600);
        SDL_SetWindowIcon(window, icon);

        scale_x = 1;
        scale_y = 1;

        fullscreen = false;
    }
}
*/