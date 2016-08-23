#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
//#include <fontconfig/fontconfig.h>
#include <stdio.h>
#include <memory>
#include <vector>
#include <iostream>
#include <cstdint>
#include <string>
#include <unistd.h>
#include <limits.h>
#ifdef _WIN32
#include <windows.h>
#endif
//#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/path.hpp>

#include "app.h"
#include "drawObject.h"
#include "events.h"
#include "scene.h"
#include "layout.h"

int WINDOW_WIDTH;
int WINDOW_HEIGHT;
float DELTA_TIME;

App::App(void) {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    }
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        printf("Warning: Linear texture filtering not enabled!");
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    }
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    }
    /*
    if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1) < 0) {
        printf("Could not set multisampling buffers for SDL GL! SDL Error: %s\n", SDL_GetError());
    }
    if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2) < 0) {
        printf("Could not set multisampling samples for SDL GL! SDL Error: %s\n", SDL_GetError());
    }
    */

    // Setup SDL window
    window = SDL_CreateWindow(
        "jmoon11",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
    }
    WINDOW_WIDTH = SDL_GetWindowSurface(window)->w;
    WINDOW_HEIGHT = SDL_GetWindowSurface(window)->h;

    // Setup SDL renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // | SDL_RENDERER_TARGETTEXTURE);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    }
    //SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);

    // Load window icon
    SDL_Surface* icon = IMG_Load(iconPath);
    if (icon == NULL) {
        printf("Unable to load texture %s! SDL Error: %s\n", iconPath, SDL_GetError());
    }
    SDL_SetWindowIcon(window, icon);
}

void App::Init() {
    #ifdef __linux__ 
        char buff[PATH_MAX];
        ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
        if (len != -1) {
          buff[len] = '\0';
          appPath = std::string(buff);
        }
    #elif _WIN32
        char ownPth[MAX_PATH]; 
        // Will contain exe path
        HMODULE hModule = GetModuleHandle(NULL);
        if (hModule != NULL)
        {
         // When passing NULL to GetModuleHandle, it returns handle of exe itself
         GetModuleFileName(hModule,ownPth, (sizeof(ownPth))); 

         // Use above module handle to get the path using GetModuleFileName()
         std::cout << ownPth << std::endl;
         appPath = std::string(ownPth);
        }
        else
        {
            std::cout << "Module handle is NULL" << std::endl ;
        }
    #else
    #endif
    // Get executable path (Linux)
    /*
    
    */

    /*
    // Setup FontConfig
    std::string fontPath = appPath.substr(0, appPath.find_last_of("\\/")) + "/fonts/";
    const FcChar8* dir = (const FcChar8*)fontPath.c_str();
    if (!FcConfigAppFontAddDir(FcConfigGetCurrent(), dir)) {
        printf("Unable to add font to FontConfig library!");
    }
    */

    // Setup scene
    scene.Init("data/test2.xml", &layout);
    layout.Init(&scene, renderer);
    //currentScene.Load("data/test1.yaml");

    // Misc objects
    auto fpsLabel = std::make_shared<FPS>(FPS(
        renderer,
        {10, 10, 0, 0}));
    miscObjects.push_back(fpsLabel);

    for (auto it = miscObjects.begin(); it != miscObjects.end(); ++it) {
        (*it)->Init();
    }
    /*
    //defaultFont = TTF_OpenFont("fonts/RomanSerif.ttf", 14);
    std::string defaultText = "Praesent in <span foreground='blue'>augue maximus</span>, <b>ultricies turpis</b> a, suscipit nulla. Aliquam quis semper felis. Donec blandit neque id accumsan facilisis. Aliquam efficitur turpis eget semper fermentum. Vivamus condimentum suscipit ligula, eget efficitur nunc volutpat quis. Cras molestie tortor libero, vitae iaculis ex hendrerit nec. Pellentesque sagittis ante id vulputate ultricies. Vivamus sollicitudin cursus dui et iaculis. Fusce eu lacus pharetra, tincidunt nulla a, malesuada nibh. Morbi maximus vestibulum nulla at semper. Donec suscipit tincidunt ex non euismod. Nulla mi nunc, sollicitudin sed arcu vel, lacinia ultrices libero. Proin orci augue, semper tempus tempus a, consectetur nec diam. Quisque tempor elit orci, vel fermentum erat pellentesque et. Vestibulum sed leo facilisis metus interdum tempus. Proin ultrices nec ipsum a semper.";
    auto hello = std::make_shared<Text>(Text(
        renderer,
        defaultText,
        {0, 0, 400, 400},
        "RomanSerif 12",
        {255, 255, 255, 255},
        ANCHOR_TOP_LEFT,
        true));
    objects.push_back(hello);
    
    auto fpsLabel = std::make_shared<FPS>(FPS(
        renderer,
        {10, 10, 0, 0}));
    objects.push_back(fpsLabel);

    auto textBox = std::make_shared<TextBox>(TextBox(
        renderer,
        defaultText,
        {10, 300, 400, 200},
        "CaviarDreams 10",
        {255, 255, 255, 255},
        {255, 96, 96, 255},
        10,
        10));
    objects.push_back(textBox);

    auto textBox2 = std::make_shared<TextBox>(TextBox(
        renderer,
        defaultText,
        {200, 150, 400, 200},
        "RomanSerif 14",
        {255, 255, 255, 255},
        {96, 96, 255, 128},
        20,
        30));
    objects.push_back(textBox2);

    auto ellipse = std::make_shared<Shape>(Shape(
        renderer,
        {400, 300, 100, 100},
        ELLIPSE,
        {255, 0, 0, 255},
        ANCHOR_TOP_LEFT));
    objects.push_back(ellipse);
    
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        (*it)->Init();
    }
    */
}

void App::Close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    IMG_Quit();
    SDL_Quit();
    //printf("closed app\n");
}

void App::ProgramLoop() {
    int fpsTimer = SDL_GetTicks();
    int frames = 0;
    uint32_t nextTick = SDL_GetTicks();
    int frameSkips;
    //double deltaTime;
    while (running) {
        frameSkips = 0;
        // Update at constant rate. Skip frames if lagging behind.
        while (SDL_GetTicks() > nextTick && frameSkips < MAX_FRAMESKIP) {
            //DELTA_TIME = SDL_GetTicks() - nextTick
            //frameRate = SDL_GetTicks() * UPDATES_PER_SECOND;
            HandleInput();
            if (SDL_GetTicks() - fpsTimer > 250 && frames > 10)
            {
                frameRate = (int)((float) frames / ((float)(SDL_GetTicks() - fpsTimer) / 1000.0f));
                //printf("%i\n", frames);
                fpsTimer = SDL_GetTicks();
                frames = 0;
                HandleFrameRate();
            }
            Update();
            nextTick += SKIP_TICKS;
            frameSkips++;
            //framesSinceUpdate = 0;
        }
        //interpolation = float(SDL_GetTicks() + SKIP_TICKS - nextTick) / float(SKIP_TICKS);
        DELTA_TIME = float(SDL_GetTicks() + SKIP_TICKS - nextTick) / 1000.0f;

        Render();
        frames++;
    }
}

void App::HandleFrameRate() {
    SDL_Event fps;
    fps.type = SDL_USEREVENT;
    fps.user.code = (int)FRAME_RATE_EVENT;
    fps.user.data1 = &frameRate;
    events.push_back(fps);
}

void App::HandleInput() {
    events.clear();
    while (SDL_PollEvent(&event) != 0) {
        //User requests quit
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else {
            events.push_back(event);
        }
    }
}

void App::Update() {
    layout.Update(events);
    for (auto it = miscObjects.begin(); it != miscObjects.end(); ++it) {
        (*it)->Update(events);
    }
}

void App::Render() {
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);
    layout.Render();
    for (auto it = miscObjects.begin(); it != miscObjects.end(); ++it) {
        (*it)->Render();
    }
    SDL_RenderPresent(renderer);
}