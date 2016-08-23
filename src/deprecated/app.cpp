#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "app.h"

App::App() {
    /*
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    window = SDL_CreateWindow(
        "concept-6",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );
    SDL_SetWindowIcon(window, IMG_Load("gfx/icon.png"));
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    */
}

//App::~App() {}

/* ******************************************** */

void App::ProgramLoop() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(
        "concept-6",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    while (running) {
        HandleInput();
        Update();
        Draw();
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    IMG_Quit();
    SDL_Quit();
}

//App::LoadState() {}

void App::HandleInput() {
    while (SDL_PollEvent(event) != 0) {
        //User requests quit
        if( event->type == SDL_QUIT )
        {
            running = false;
        }
    }
}

void App::Update() {

}

void App::Draw() {
    /*
    for(std::vector<T>::iterator it = v.begin(); it != v.end(); ++it) {
    }
    */
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}