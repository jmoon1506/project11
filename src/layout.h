#ifndef LAYOUT_H
#define LAYOUT_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>

#include "drawObject.h"
#include "scene.h"

class Scene;

class Layout {
 public:
    Layout();
    void Init(Scene* scene, SDL_Renderer* renderer);
    void Update(const std::vector<SDL_Event>& events);
    void Render();
    void AddText(std::string text);
    void AddResponse(std::string response);
    void RetargetObjects();
    void FocusOn(int index);

    SDL_Renderer* m_renderer;
    //std::shared_ptr<DrawObject>* m_lastVisible;
    std::vector<std::shared_ptr<TextBox>> m_dialogueObjects;
    Scene* m_scene;
    static const int m_dialogueMarginX = 30;
    static const int m_dialogueMarginY = 30;
    int m_newTextY = 0;
    //int m_focusedDialogue = 0;
    int m_firstVisible = 0; // top visible textbox
    int m_lastVisible = 0;  // bottom visible textbox
    TTF_Font* defaultFont;
};

#endif