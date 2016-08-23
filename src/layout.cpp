#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <iostream>

#include "layout.h"
#include "events.h"
#include "drawObject.h"
#include "scene.h"
#include "app.h"

Layout::Layout() {}

void Layout::Init(Scene* scene, SDL_Renderer* renderer) {
    m_scene = scene;
    m_renderer = renderer;
    defaultFont = TTF_OpenFont("fonts/Arcon-Regular.otf", 24);
    if (defaultFont == NULL) {
        printf("Unable to load default font: %s\n", TTF_GetError());
    }
}

void Layout::Update(const std::vector<SDL_Event>& events) {
    for (auto it = events.begin(); it != events.end(); ++it) {
        if (it->type == SDL_KEYUP) {
            switch (it->key.keysym.sym) {
                case SDLK_SPACE:
                    m_scene->AdvanceNode();
                    //RetargetObjects();
                    break;
                case SDLK_UP:
                    FocusOn(m_firstVisible - 1);
                    //RetargetObjects();
                    break;
                case SDLK_DOWN:
                    FocusOn(m_firstVisible + 1);
                    //RetargetObjects();
                    break;
            }
        }
    }
    for (auto it = m_dialogueObjects.begin(); it != m_dialogueObjects.end(); ++it) {
        (*it)->Update(events);
    }
}

void Layout::Render() {
    for (auto it = m_dialogueObjects.begin(); it != m_dialogueObjects.end(); ++it) {
        (*it)->Render();
    }
}

void Layout::FocusOn(int focusIndex) {
    if (focusIndex < 0 || focusIndex >= m_dialogueObjects.size()) {
        return;
    }
    m_firstVisible = focusIndex;
    int placementY = 0;
    //float shift = (float)(m_dialogueObjects[m_firstVisible]->m_rect.y - m_dialogueObjects[focusIndex]->m_rect.y);
    unsigned int q = focusIndex;
    while (q < m_dialogueObjects.size()) {
        placementY += m_dialogueMarginY;
        if (placementY + m_dialogueObjects[q]->m_rect.h > WINDOW_HEIGHT - m_dialogueMarginY) {
            break;  // escape if element overflows window margin
        }
        m_dialogueObjects[q]->m_dampTarget.y = placementY;
        placementY += m_dialogueObjects[q]->m_rect.h;
        q++;
    }
    m_lastVisible = q - 1;
    for (size_t i = 0; i < (unsigned int)focusIndex; i++) {
        m_dialogueObjects[i]->m_dampTarget.y = -WINDOW_HEIGHT;
    }
    if (m_lastVisible + 1 < m_dialogueObjects.size()) {
        for (size_t j = m_lastVisible + 1; j < m_dialogueObjects.size(); j++) {
            m_dialogueObjects[j]->m_dampTarget.y = 2 * WINDOW_HEIGHT;
        }
    }
}

void Layout::AddText(std::string text) {
    m_newTextY = m_dialogueMarginY; // move cursor down by margin
    if (m_dialogueObjects.size() > 0)   // move cursor down by last visible y if it exists
        m_newTextY += m_dialogueObjects[m_lastVisible]->m_dampTarget.y + m_dialogueObjects[m_lastVisible]->m_rect.h;
        //printf("last vis y %i\n", m_newTextY);
    auto textBox = std::make_shared<TextBox>(TextBox(
        m_renderer,
        text,
        {m_dialogueMarginX, m_newTextY, WINDOW_WIDTH - 2 * m_dialogueMarginX, WINDOW_HEIGHT},
        //{m_dialogueMarginX, m_newDialoguePositionY, 400, newDialogueHeight},
        defaultFont,
        {255, 255, 255, 255},
        {0, 0, 0, 255},
        20,
        20));
    textBox->Init();
    m_dialogueObjects.push_back(textBox);
    m_lastVisible = m_dialogueObjects.size() - 1;   // set last visible to newly created textbox
    //printf("new text y %i\n", m_newTextY);
    if (m_newTextY + textBox->m_rect.h > WINDOW_HEIGHT - m_dialogueMarginY) {   // refocus if new textbox overflows
        FocusOn(m_lastVisible - 1);
    }
    //printf("position %i\n", m_newTextY + textBox->m_rect.h);
}
/*
void Layout::AddText(std::string text) {
    //m_newDialoguePositionY += m_dialogueMarginY; // shift new position forward by margin
    //int newDialogueHeight = WINDOW_HEIGHT;    // TODO: adjust to box text

    // Shift to focus if last dialogue is not visible
    //if (!m_dialogueObjects[m_dialogueObjects.size() - 1]->IsVisible()) {
    //    m_focusedDialogue = m_dialogueObjects.size() - 1;
    //    ShiftToFocus();
    //}
    
    // Add new text
    auto textBox = std::make_shared<TextBox>(TextBox(
        m_renderer,
        text,
        {m_dialogueMarginX, m_newDialoguePositionY, WINDOW_WIDTH - 2 * m_dialogueMarginX, WINDOW_HEIGHT},
        //{m_dialogueMarginX, m_newDialoguePositionY, 400, newDialogueHeight},
        "CrimsonText 18",
        {255, 255, 255, 255},
        {0, 0, 0, 255},
        20,
        20));
    textBox->Init();
    m_dialogueObjects.push_back(textBox);

    m_newDialoguePositionY += textBox->m_rect.h;
    if (m_newDialoguePositionY > (WINDOW_HEIGHT - m_dialogueMarginY)) {
        RetargetObjects();
    }
}

void Layout::RetargetObjects() {
    int dialoguesKept;
    float shift;
    int lastObjectHeight = m_dialogueObjects[m_dialogueObjects.size() - 1]->m_rect.h;
    if (m_dialogueObjects.size() < 3) {
        dialoguesKept = 1;
        shift = m_newDialoguePositionY - m_dialogueMarginY - lastObjectHeight;
        m_newDialoguePositionY = m_dialogueMarginY + lastObjectHeight;
    } else {
        dialoguesKept = 2;
        int secondToLastObjectHeight = m_dialogueObjects[m_dialogueObjects.size() - 2]->m_rect.h;
        shift = m_newDialoguePositionY - 2 * m_dialogueMarginY - lastObjectHeight - secondToLastObjectHeight;
        m_newDialoguePositionY = 2 * m_dialogueMarginY + lastObjectHeight + secondToLastObjectHeight;
        m_dialogueObjects[m_dialogueObjects.size() - 2]->m_dampTarget.y -= shift;
        m_dialogueObjects[m_dialogueObjects.size() - 2]->m_smoothTime = 2.0f;
    }
    for (auto it = m_dialogueObjects.begin(); it != m_dialogueObjects.end() - dialoguesKept; ++it) {
        (*it)->m_dampTarget.y -= shift + 300.0f;
        (*it)->m_smoothTime = 2.0f;
    }
    m_dialogueObjects[m_dialogueObjects.size() - 1]->m_dampTarget.y -= shift;
    m_dialogueObjects[m_dialogueObjects.size() - 1]->m_smoothTime = 2.0f;
}
*/
