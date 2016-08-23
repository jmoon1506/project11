#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <memory>

//#include "drawObject.h"
#include "scene.h"
#include "layout.h"

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern float DELTA_TIME;    // time in seconds to complete last frame

class App {
 public:
    App();
    void Init();
    void ProgramLoop();
    void HandleFrameRate();
    void HandleInput();
    void Update();
    void Render();
    void Close();

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    
    std::string appPath = "";
    std::vector<std::shared_ptr<DrawObject>> miscObjects;
    std::vector<SDL_Event> events;
    TTF_Font* defaultFont;

    Scene scene;
    Layout layout;

 private:
    bool running = true;
    char const* iconPath = "gfx/icon.png";
    const int UPDATES_PER_SECOND = 120;
    const int SKIP_TICKS = 1000 / UPDATES_PER_SECOND;
    const int MAX_FRAMESKIP = 5;
    //const int MAX_FRAMERATE = 1000
    int frameRate = 0;
    //uint8_t frameRateEvent;
    //float interpolation;
};

#endif
