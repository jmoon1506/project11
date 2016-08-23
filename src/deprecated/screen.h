#ifndef SCREEN_H
#define SCREEN_H

class Screen {
 public:
    bool fullscreen;
    int width;
    int height;
    SDL_Surface* icon;

    Screen();
    void Init(SDL_Window* wndw);
    void Resize(SDL_Window* wndw, int w, int h);
    void Fullscreen(SDL_Window* wndw, bool fulscr);
};

#endif
