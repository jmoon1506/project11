#ifndef DRAWOBJECT_H
#define DRAWOBJECT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

typedef enum {
    ANCHOR_TOP_LEFT = 0,
    ANCHOR_TOP_RIGHT,
    ANCHOR_BOTTOM_LEFT,
    ANCHOR_BOTTOM_RIGHT
} AnchorPosition;

typedef struct {
    double x;
    double y;
} Point;

typedef enum {
    ELLIPSE = 0,
    RECTANGLE
} ShapeType;

class DrawObject {
 public:
    DrawObject(SDL_Renderer* rndr,
        SDL_Rect rect = {0, 0, 100, 100},
        AnchorPosition anchor = ANCHOR_TOP_LEFT);
    virtual void Init() = 0;
    virtual void Update(const std::vector<SDL_Event>& evnts) = 0;
    virtual void Render() = 0;
    virtual void Free() = 0;
    bool IsVisible();

    SDL_Texture* m_texture = NULL;
    SDL_Renderer* m_renderer = NULL;
    SDL_Rect m_rect;
    AnchorPosition m_anchorPosition;
};

class Shape : public DrawObject {
 public:
    Shape(SDL_Renderer* rndr,
        SDL_Rect rect,
        ShapeType type,
        SDL_Color color = {255, 255, 255, 255},
        AnchorPosition anchor = ANCHOR_TOP_LEFT);
    void Init();
    void Update(const std::vector<SDL_Event>& evnts);
    void Render();
    void Free();
    void Draw();

    ShapeType m_type;
    SDL_Color m_color;
};

class Text : public DrawObject {
 public:
    explicit Text(SDL_Renderer* rndr,
        std::string txt,
        SDL_Rect rect,
        TTF_Font* font,
        SDL_Color color = {255, 255, 255, 255},
        AnchorPosition anchor = ANCHOR_TOP_LEFT,
        bool lineWrap = true);
    void Init();
    void Draw();
    void Update(const std::vector<SDL_Event>& evnts);
    void Render();
    void Free();

    std::string m_text;
    SDL_Color m_color;
    TTF_Font* m_font;
    //bool overflow;
    bool m_lineWrap;
 protected:
    //bool DrawText();
};

class FPS : public DrawObject {
 public:
    explicit FPS(SDL_Renderer* rndr,
        SDL_Rect rect);
    void Init();
    void Draw();
    void Update(const std::vector<SDL_Event>& evnts);
    void Render();
    void Free();
 private:
    TTF_Font* m_font;
    std::string m_text;
    //int skipUpdates = 0;
};

class TextBox : public DrawObject {
 public:
    explicit TextBox(SDL_Renderer* rndr,
        std::string txt,
        SDL_Rect rect,
        TTF_Font* font,
        SDL_Color txtColor = {255, 255, 255, 255},
        SDL_Color boxColor = {96, 96, 96, 255},
        int boxMargin = 30,
        int boxCornerRadius = 20);
    void Init();
    void Draw();
    void Update(const std::vector<SDL_Event>& evnts);
    void Render();
    void Free();
    void Retarget(Point target);
    double Damp(double current, double target, double& currentVelocity);
    //static constexpr float SMOOTH_TIME = 2.0f;

    std::string m_text;
    TTF_Font* m_font;
    //double m_fontDPI = 192.0;
    int m_line_spacing = 5;
    
    SDL_Color m_textColor;
    SDL_Color m_boxColor;
    int m_boxMargin;
    int m_boxCornerRadius;

    Point m_dampTarget;
    Point m_dampVelocity = { 0.0, 0.0 };
    double m_smoothTime = 0.2;
    //float m_maxSpeed = 1.0f;
};

class ScrollBar : public DrawObject {

};

/*
class DialogueText : public DrawObject {
    std::string m_text;
};

class DialogueResponse : public DrawObject {
    std::string m_text;
    std::shared_ptr<DialogueNode>* m_node;
    bool active;
};
*/

#endif