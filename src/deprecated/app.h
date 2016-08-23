#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
//#include <vector>
//#include "drawObject.h"
//#include "state.h"

class App {
private:
    //Screen screen;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event* event;

public:
    App();
    //~App();

    void ProgramLoop();
    void HandleInput();
    void Update();
    void Draw();

    bool running = true;
    //std::vector<DrawObject*> drawObjects;
    //State state;
    //static bool mouseLeftPressed, mouseRightPressed;
    //static int mouseX, mouseY;
};

#endif
