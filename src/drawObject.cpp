#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
//#include <pango/pangocairo.h>
//#include <cairo/cairo.h>
//#include <fontconfig/fontconfig.h>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
 #include <cmath>

#include "app.h"
#include "drawObject.h"
#include "events.h"
#include "primitives.h"

/*
Summary: Abstract base object for rendering and updates.
*/
DrawObject::DrawObject(SDL_Renderer* rndr, SDL_Rect rect, AnchorPosition anchor) :
    m_renderer(rndr),
    m_rect(rect),
    m_anchorPosition(anchor) {}

void DrawObject::Render() {
    SDL_Rect renderQuad;
    if (m_anchorPosition == ANCHOR_TOP_LEFT) {
        renderQuad.x = m_rect.x;
        renderQuad.y = m_rect.y;
    } else if (m_anchorPosition == ANCHOR_TOP_RIGHT) {
        renderQuad.x = WINDOW_WIDTH - m_rect.x - m_rect.w;
        renderQuad.y = m_rect.y;
    } else if (m_anchorPosition == ANCHOR_BOTTOM_LEFT) {
        renderQuad.x = m_rect.x;
        renderQuad.y = WINDOW_HEIGHT - m_rect.y - m_rect.h;
    } else if (m_anchorPosition == ANCHOR_BOTTOM_RIGHT) {
        renderQuad.x = WINDOW_WIDTH - m_rect.x - m_rect.w;
        renderQuad.y = WINDOW_HEIGHT - m_rect.y - m_rect.h;
    } else {
        throw std::invalid_argument("Object has no anchor position.");
    }
    renderQuad.w = m_rect.w;
    renderQuad.h = m_rect.h;
    SDL_Rect clip = { 0, 0, m_rect.w, m_rect.h };
    SDL_RenderCopy(m_renderer, m_texture, &clip, &renderQuad);
}

void DrawObject::Free() {
    if (m_texture != NULL) {
        SDL_DestroyTexture(m_texture);
        m_texture = NULL;
    }
}

bool DrawObject::IsVisible() {
    return !((m_rect.x + m_rect.w < 0 || m_rect.x > WINDOW_WIDTH) ||
            (m_rect.y + m_rect.h < 0 || m_rect.y > WINDOW_HEIGHT));
}

/*
Summary: Geometric primitive ellipse, rectangle, or rounded rectangle.
*/
Shape::Shape(SDL_Renderer* rndr,
        SDL_Rect rect,
        ShapeType type,
        SDL_Color color,
        AnchorPosition anchor) :
            DrawObject(rndr, rect, anchor), m_type(type), m_color(color) {}

void Shape::Init() {
    Free();
    Draw();
}

void Shape::Draw() {
    m_texture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        m_rect.w,
        m_rect.h);
    SDL_SetRenderTarget(m_renderer, m_texture);
    if (m_type == ELLIPSE) {
        if (Primitives::DrawEllipse(
            m_renderer,
            m_rect.w / 2,
            m_rect.h / 2,
            m_rect.w / 2 - 1,
            m_rect.h / 2 - 1,
            m_color)
        < 0) {
            printf("Ellipse could not be created! SDL Error: %s\n", SDL_GetError());
        }
    }
    else if (m_type == RECTANGLE) {
        if (Primitives::DrawRectangle(
            m_renderer,
            0,
            0,
            m_rect.w - 1,
            m_rect.h - 1,
            m_color)
        < 0) {
            printf("Ellipse could not be created! SDL Error: %s\n", SDL_GetError());
        }
    }
    SDL_SetRenderTarget(m_renderer, NULL);
    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
}

void Shape::Update(const std::vector<SDL_Event>& evnts) {
}

void Shape::Render() {
    DrawObject::Render();
}

void Shape::Free() {
    DrawObject::Free();
}

/*
Summary: Blended text rendering.
*/
Text::Text(SDL_Renderer* rndr,
    std::string txt,
    SDL_Rect rect,
    TTF_Font* font,
    SDL_Color color,
    AnchorPosition anchor,
    bool lineWrap) :
        DrawObject(rndr, rect, anchor),
        m_text(txt), m_color(color), m_font(font), m_lineWrap(lineWrap) {}

void Text::Init() {
    Free();
    Draw();
}

void Text::Draw() {
    
    SDL_Surface* surface;
    if (m_lineWrap) {
        surface = TTF_RenderText_Blended_Wrapped(m_font, m_text.c_str(), m_color, m_rect.w);
    } else {
        surface = TTF_RenderText_Blended(m_font, m_text.c_str(), m_color);
    }
    if (surface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    if (m_texture == NULL) {
        printf("Unable to create texture from text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    m_rect.w = surface->w;
    m_rect.h = surface->h;
    SDL_FreeSurface(surface);
    
    /*
    m_texture = SDL_CreateTexture(m_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        m_rect.w, m_rect.h);

    void* pixels;
    int pitch;
    SDL_LockTexture(m_texture, NULL, &pixels, &pitch);
    cairo_surface_t* cairo_surface = cairo_image_surface_create_for_data(
        (unsigned char*)pixels,
        CAIRO_FORMAT_ARGB32,
        m_rect.w, m_rect.h, pitch);
    cairo_t* cr = cairo_create(cairo_surface);
    
    // Text generation using Pango
    PangoLayout* layout = pango_cairo_create_layout(cr);
    PangoFontDescription* desc = pango_font_description_from_string(m_font.c_str());
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);
    pango_layout_set_width(layout, m_rect.w * PANGO_SCALE);
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_markup(layout, m_text.c_str(), -1);
    cairo_set_source_rgba(cr, (double)m_color.r / 255.0, (double)m_color.g / 255.0, (double)m_color.b / 255.0, (double)m_color.a / 255.0);
    cairo_move_to(cr, 0, 0);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);
    cairo_destroy(cr);
    cairo_surface_destroy(cairo_surface);
    SDL_UnlockTexture(m_texture);
    */
    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
}

void Text::Update(const std::vector<SDL_Event>& evnts) {
}

void Text::Render() {
    DrawObject::Render();
}

void Text::Free() {
    DrawObject::Free();
}

/*
Summary: Frame rate label.
*/
FPS::FPS(SDL_Renderer* rndr,
    SDL_Rect rect) :
        DrawObject(rndr, rect, ANCHOR_TOP_RIGHT), m_text("0") {}

void FPS::Init() {
    m_font = TTF_OpenFont("fonts/Hack-Regular.ttf", 16);
    if (m_font == NULL) {
        printf("Unable to load FPS font: %s\n", TTF_GetError());
    }
    Free();
    Draw();
}

void FPS::Update(const std::vector<SDL_Event>& evnts) {
    /*
    if (skipUpdates < 10) {
        skipUpdates++;
        return;
    }
    */
    for (auto it = evnts.begin(); it != evnts.end(); ++it) {
        if (it->type == SDL_USEREVENT) {
            if (it->user.code == FRAME_RATE_EVENT) {
                m_text = std::to_string(*(int*)it->user.data1) + " fps";
            }
        }
    }
    Free();
    Draw();
    //skipUpdates = 0;
}

void FPS::Render() {
    DrawObject::Render();
}

void FPS::Free() {
    DrawObject::Free();
}

void FPS::Draw() {
    SDL_Surface* surface;
    surface = TTF_RenderText_Blended(m_font, m_text.c_str(), {255, 255, 255, 255});
    if (surface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    if (m_texture == NULL) {
        printf("Unable to create texture from text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    m_rect.w = surface->w;
    m_rect.h = surface->h;
    SDL_FreeSurface(surface);
}

/*
Summary: Rounded rectangle overlaid with text.
*/
TextBox::TextBox(SDL_Renderer* rndr,
        std::string txt,
        SDL_Rect rect,
        TTF_Font* font,
        SDL_Color txtColor,
        SDL_Color boxColor,
        int boxMargin,
        int boxCornerRadius) :
            DrawObject(rndr, rect, ANCHOR_TOP_LEFT),
            m_text(txt), m_font(font), m_textColor(txtColor), 
            m_boxColor(boxColor), m_boxMargin(boxMargin), m_boxCornerRadius(boxCornerRadius) {}

void TextBox::Init() {
    m_dampTarget = { (float)m_rect.x, (float)m_rect.y };
    //m_dampVelocity = { 0.0, 0.0 };
    //m_smoothTime = 0.5f;
    //printf("rect %i %i damp %f %f\n", m_rect.x, m_rect.y, m_dampTarget.x, m_dampTarget.y);
    //std::cout << m_dampTarget.x << " " << m_dampTarget.y << "\n";
    Free();
    Draw();
}

void TextBox::Update(const std::vector<SDL_Event>& evnts) {
  if (std::abs(m_dampTarget.y - (double)m_rect.y) > 0.0001f) {
        m_rect.y = Damp(m_rect.y, m_dampTarget.y, m_dampVelocity.y);
  }
    //printf("rect %i %i damp %f %f\n", m_rect.x, m_rect.y, m_dampTarget.x, m_dampTarget.y);
    //if (abs((float)m_rect.x - m_dampTarget.x) > 0.001f || abs((float)m_rect.y - m_dampTarget.y) > 0.001f) {
}

// Primitives from SDL_gfx

void TextBox::Draw() {
    SDL_Surface* surface;
    surface = TTF_RenderText_Blended_Wrapped(m_font, m_text.c_str(), m_textColor, m_rect.w - 2 * m_boxMargin);
    if (surface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    if (text_texture == NULL) {
        printf("Unable to create texture from text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }

    m_rect.w = surface->w + 2 * m_boxMargin;
    m_rect.h = surface->h + 2 * m_boxMargin;
    m_texture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        m_rect.w,
        m_rect.h);

    // Set renderer to texture
    SDL_SetRenderTarget(m_renderer, m_texture);
    SDL_SetRenderDrawColor(m_renderer, m_boxColor.r, m_boxColor.g, m_boxColor.b, m_boxColor.a);

    // Render rounded rectangle
    //roundedBoxRGBA(m_renderer, 0, 0, m_rect.w - 1, m_rect.h - 1, m_boxCornerRadius,
    //    m_boxColor.r, m_boxColor.g, m_boxColor.b, m_boxColor.a);
    Primitives::DrawRoundedRectangle(m_renderer, 0, 0, m_rect.w - 1, m_rect.h - 1, m_boxCornerRadius, m_boxColor);
    //Primitives::DrawEllipse(m_renderer, m_rect.w / 2, m_rect.h / 2, m_rect.w / 2 - 1, m_rect.h / 2 - 1, {0, 255, 0, 255});
    
    // Render text
    SDL_Rect renderQuad = { m_boxMargin, m_boxMargin, surface->w, surface->h };
    SDL_RenderCopy(m_renderer, text_texture, NULL, &renderQuad);

    // Free renderer and intermediate surface and texture
    SDL_SetRenderTarget(m_renderer, NULL);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(text_texture);
    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
}


/*
void TextBox::Draw() {

    SDL_Texture* drawTexture = SDL_CreateTexture(m_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        m_rect.w, m_rect.h);

    void* pixels;
    int pitch;
    SDL_LockTexture(drawTexture, NULL, &pixels, &pitch);
    cairo_surface_t* cairo_surface = cairo_image_surface_create_for_data(
        (unsigned char*)pixels,
        CAIRO_FORMAT_ARGB32,
        m_rect.w, m_rect.h, pitch);
    cairo_t* cr = cairo_create(cairo_surface);

    
    // Text generation using Pango
    PangoLayout* layout = pango_cairo_create_layout(cr);
    PangoFontDescription* desc = pango_font_description_from_string(m_font.c_str());
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);

    // Setup PangoCairo font map for kerning
    //cairo_font_face_t* cairoFontFace = cairo_get_font_face(cr);
    //cairo_font_type_t cairoFontType = cairo_font_face_get_type(cairoFontFace);
    //PangoCairoFontMap* fontMap = (PangoCairoFontMap*)pango_cairo_font_map_new_for_font_type(cairoFontType);
    //pango_cairo_font_map_set_default(fontMap);
    //PangoContext* context = pango_font_map_create_context(fontMap);
    //pango_cairo_font_map_set_default(fontMap);

    //pango_cairo_font_map_set_resolution((PangoCairoFontMap*)fontMap, m_fontDPI);
    //PangoContext* context = pango_cairo_font_map_create_context(map);
    //pango_cairo_update_context(cr, context);
    pango_layout_set_width(layout, (m_rect.w - 2 * m_boxMargin) * PANGO_SCALE);
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_spacing(layout, m_line_spacing * PANGO_SCALE);
    pango_layout_set_markup(layout, m_text.c_str(), -1);
    //pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_NONE);

    int layoutWidth, layoutHeight;
    pango_layout_get_pixel_size(layout, &layoutWidth, &layoutHeight);
    layoutWidth = layoutWidth + 2 * m_boxMargin;
    layoutHeight = layoutHeight + 2 * m_boxMargin;
    //printf("%i %i\n", m_rect.w, m_rect.h);

    //pango_layout_set_text(layout, m_text.c_str(), -1);

    // Rounded rectangle in Cairo
    Primitives::DrawRoundedRectangleCairo(cr, 0, 0, (double)layoutWidth, (double)layoutHeight, (double)m_boxCornerRadius, m_boxColor);
    // Text using Pango in Cairo context
    cairo_set_source_rgba(cr, (double)m_textColor.r / 255.0, (double)m_textColor.g / 255.0, (double)m_textColor.b / 255.0, (double)m_textColor.a / 255.0);
    cairo_move_to(cr, m_boxMargin, m_boxMargin);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);
    cairo_destroy(cr);
    cairo_surface_destroy(cairo_surface);
    SDL_UnlockTexture(drawTexture);
    m_rect = { m_rect.x, m_rect.y, layoutWidth, layoutHeight };
    //printf("%i %i %i %i\n", m_rect.x, m_rect.y, m_rect.w, m_rect.h);
    m_texture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        m_rect.w,
        m_rect.h);
    SDL_SetRenderTarget(m_renderer, m_texture);
    SDL_Rect srcRect = { 0, 0, m_rect.w, m_rect.h };
    SDL_RenderCopy(m_renderer, drawTexture, &srcRect, NULL);
    SDL_DestroyTexture(drawTexture);
    SDL_SetRenderTarget(m_renderer, NULL);
    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
}
*/

void TextBox::Render() {
    
    if (DrawObject::IsVisible()) {
        DrawObject::Render();
    }
}

void TextBox::Free() {
    DrawObject::Free();
}

double TextBox::Damp(double current, double target, double& currentVelocity) {
    //smoothTime = std::max(0.0001f, m_smoothTime);
    double num = 2.0f / m_smoothTime;
    double num2 = num * DELTA_TIME;
    double num3 = 1.0f / (1.0f + num2 + 0.48f * num2 * num2 + 0.235f * num2 * num2 * num2);
    double num4 = current - target;
    double num5 = target;
    //double num6 = m_maxSpeed * m_smoothTime;
    //num4 = std::min(num6, std::max(num4, -num6));
    target = current - num4;
    double num7 = (currentVelocity + num * num4) * DELTA_TIME;
    currentVelocity = (currentVelocity - num * num7) * num3;
    double num8 = target + (num4 + num7) * num3;
    if ((num5 - current > 0.0f) == (num8 > num5))
    {
        num8 = num5;
        currentVelocity = (num8 - num5) / DELTA_TIME;
    }
    return num8;
}
