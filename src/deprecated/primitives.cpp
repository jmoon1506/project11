#include <SDL2/SDL.h>
#include <math.h>
#include <cairo/cairo.h>
#include <pango/pangocairo.h>

#include "primitives.h"


int Primitives::PixelRGBA(SDL_Renderer* renderer, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    int result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);
    result |= SDL_RenderDrawPoint(renderer, x, y);
    return result;
}

int Primitives::PixelRGBAWeight(SDL_Renderer* renderer, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint32_t weight) {
    /*
    * Modify Alpha by weight 
    */
    uint32_t ax = a;
    ax = ((ax * weight) >> 8);
    if (ax > 255) {
        a = 255;
    } else {
        a = (Uint8)(ax & 0x000000ff);
    }
    return PixelRGBA(renderer, x, y, r, g, b, a);
}

int Primitives::HLine(SDL_Renderer * renderer, int x1, int x2, int y)
{
    return SDL_RenderDrawLine(renderer, x1, y, x2, y);
}

// Anti-aliased outline

int Primitives::DrawEllipse(SDL_Renderer* renderer, int x, int y, int rx, int ry, SDL_Color color) {
    int result;
    int i;
    int a2, b2, ds, dt, dxt, t, s, d;
    int xp, yp, xs, ys, dyt, od, xx, yy, xc2, yc2;
    float cp;
    double sab;
    uint8_t weight, iweight;
    uint8_t r, g, b, a;

    /*
    * Sanity check radii 
    */
    if ((rx <= 0) || (ry <= 0)) {
        return (-1);
    }

    /* Variable setup */
    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;

    a2 = rx * rx;
    b2 = ry * ry;

    ds = 2 * a2;
    dt = 2 * b2;

    xc2 = 2 * x;
    yc2 = 2 * y;

    sab = sqrt((double)(a2 + b2));
    od = (int)lrint(sab*0.01) + 1; /* introduce some overdraw */
    dxt = (int)lrint((double)a2 / sab) + od;

    t = 0;
    s = -2 * a2 * ry;
    d = 0;

    xp = x;
    yp = y - ry;

    /* Draw */
    result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    //result |= SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    /* "End points" */
    result |= PixelRGBA(renderer, xp, yp, r, g, b, a);
    result |= PixelRGBA(renderer, xc2 - xp, yp, r, g, b, a);
    result |= PixelRGBA(renderer, xp, yc2 - yp, r, g, b, a);
    result |= PixelRGBA(renderer, xc2 - xp, yc2 - yp, r, g, b, a);

    for (i = 1; i <= dxt; i++) {
        xp--;
        d += t - b2;

        if (d >= 0)
            ys = yp - 1;
        else if ((d - s - a2) > 0) {
            if ((2 * d - s - a2) >= 0)
                ys = yp + 1;
            else {
                ys = yp;
                yp++;
                d -= s + a2;
                s += ds;
            }
        } else {
            yp++;
            ys = yp + 1;
            d -= s + a2;
            s += ds;
        }

        t -= dt;

        /* Calculate alpha */
        if (s != 0) {
            cp = (float) abs(d) / (float) abs(s);
            if (cp > 1.0) {
                cp = 1.0;
            }
        } else {
            cp = 1.0;
        }

        /* Calculate weights */
        weight = (uint8_t) (cp * 255);
        iweight = 255 - weight;

        /* Upper half */
        xx = xc2 - xp;
        result |= PixelRGBAWeight(renderer, xp, yp, r, g, b, a, iweight);
        result |= PixelRGBAWeight(renderer, xx, yp, r, g, b, a, iweight);

        result |= PixelRGBAWeight(renderer, xp, ys, r, g, b, a, weight);
        result |= PixelRGBAWeight(renderer, xx, ys, r, g, b, a, weight);

        /* Lower half */
        yy = yc2 - yp;
        result |= PixelRGBAWeight(renderer, xp, yy, r, g, b, a, iweight);
        result |= PixelRGBAWeight(renderer, xx, yy, r, g, b, a, iweight);

        yy = yc2 - ys;
        result |= PixelRGBAWeight(renderer, xp, yy, r, g, b, a, weight);
        result |= PixelRGBAWeight(renderer, xx, yy, r, g, b, a, weight);
    }

    /* Replaces original approximation code dyt = abs(yp - yc); */
    dyt = (int)lrint((double)b2 / sab ) + od;    

    for (i = 1; i <= dyt; i++) {
        yp++;
        d -= s + a2;

        if (d <= 0)
            xs = xp + 1;
        else if ((d + t - b2) < 0) {
            if ((2 * d + t - b2) <= 0)
                xs = xp - 1;
            else {
                xs = xp;
                xp--;
                d += t - b2;
                t -= dt;
            }
        } else {
            xp--;
            xs = xp - 1;
            d += t - b2;
            t -= dt;
        }

        s += ds;

        /* Calculate alpha */
        if (t != 0) {
            cp = (float) abs(d) / (float) abs(t);
            if (cp > 1.0) {
                cp = 1.0;
            }
        } else {
            cp = 1.0;
        }

        /* Calculate weight */
        weight = (uint8_t) (cp * 255);
        iweight = 255 - weight;

        /* Left half */
        xx = xc2 - xp;
        yy = yc2 - yp;
        result |= PixelRGBAWeight(renderer, xp, yp, r, g, b, a, iweight);
        result |= PixelRGBAWeight(renderer, xx, yp, r, g, b, a, iweight);

        result |= PixelRGBAWeight(renderer, xp, yy, r, g, b, a, iweight);
        result |= PixelRGBAWeight(renderer, xx, yy, r, g, b, a, iweight);

        /* Right half */
        xx = xc2 - xs;
        result |= PixelRGBAWeight(renderer, xs, yp, r, g, b, a, weight);
        result |= PixelRGBAWeight(renderer, xx, yp, r, g, b, a, weight);

        result |= PixelRGBAWeight(renderer, xs, yy, r, g, b, a, weight);
        result |= PixelRGBAWeight(renderer, xx, yy, r, g, b, a, weight);        
    }

    return (result);
}

int Primitives::DrawRoundedRectangle(SDL_Renderer* renderer, int x1, int y1, int x2,
    int y2, int rad, SDL_Color color)
{
    int result;
    int w, h, r2, tmp;
    int cx = 0;
    int cy = rad;
    int ocx = (int) 0xffff;
    int ocy = (int) 0xffff;
    int df = 1 - rad;
    int d_e = 3;
    int d_se = -2 * rad + 5;
    int xpcx, xmcx, xpcy, xmcy;
    int ypcy, ymcy, ypcx, ymcx;
    int x, y, dx, dy;
    uint8_t r, g, b, a;

    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;

    /* 
    * Check destination renderer 
    */
    if (renderer == NULL)
    {
        return -1;
    }

    /*
    * Check radius vor valid range
    */
    if (rad < 0) {
        return -1;
    }

    /*
    * Special case - no rounding
    */
    if (rad <= 1) {
        return DrawRectangle(renderer, x1, y1, x2, y2, {r, g, b, a});
    }

    /*
    * Swap x1, x2 if required 
    */
    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    /*
    * Swap y1, y2 if required 
    */
    if (y1 > y2) {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    /*
    * Calculate width&height 
    */
    w = x2 - x1 + 1;
    h = y2 - y1 + 1;

    /*
    * Maybe adjust radius
    */
    r2 = rad + rad;
    if (r2 > w)  
    {
        rad = w / 2;
        r2 = rad + rad;
    }
    if (r2 > h)
    {
        rad = h / 2;
    }

    /* Setup filled circle drawing for corners */
    x = x1 + rad;
    y = y1 + rad;
    dx = x2 - x1 - rad - rad;
    dy = y2 - y1 - rad - rad;

    /*
    * Set color
    */
    result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);

    /*
    * Draw corners
    */
    do {
        xpcx = x + cx;
        xmcx = x - cx;
        xpcy = x + cy;
        xmcy = x - cy;
        if (ocy != cy) {
            if (cy > 0) {
                ypcy = y + cy;
                ymcy = y - cy;
                result |= HLine(renderer, xmcx, xpcx + dx, ypcy + dy);
                result |= HLine(renderer, xmcx, xpcx + dx, ymcy);
            } else {
                result |= HLine(renderer, xmcx, xpcx + dx, y);
            }
            ocy = cy;
        }
        if (ocx != cx) {
            if (cx != cy) {
                if (cx > 0) {
                    ypcx = y + cx;
                    ymcx = y - cx;
                    result |= HLine(renderer, xmcy, xpcy + dx, ymcx);
                    result |= HLine(renderer, xmcy, xpcy + dx, ypcx + dy);
                } else {
                    result |= HLine(renderer, xmcy, xpcy + dx, y);
                }
            }
            ocx = cx;
        }

        /*
        * Update 
        */
        if (df < 0) {
            df += d_e;
            d_e += 2;
            d_se += 2;
        } else {
            df += d_se;
            d_e += 2;
            d_se += 4;
            cy--;
        }
        cx++;
    } while (cx <= cy);

    /* Inside */
    if (dx > 0 && dy > 0) {
        result |= DrawRectangle(renderer, x1, y1 + rad + 1, x2, y2 - rad, {r, g, b, a});
    }

    return (result);
}

int Primitives::DrawRectangle(SDL_Renderer * renderer, int x1, int y1, int x2, int y2, SDL_Color color)
{
    uint8_t r = color.r;
    uint8_t g = color.g;
    uint8_t b = color.b;
    uint8_t a = color.a;

    int result;
    int tmp;
    SDL_Rect rect;

    /*
    * Swap x1, x2 if required 
    */
    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    /*
    * Swap y1, y2 if required 
    */
    if (y1 > y2) {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    /* 
    * Create destination rect
    */  
    rect.x = x1;
    rect.y = y1;
    rect.w = x2 - x1 + 1;
    rect.h = y2 - y1 + 1;
    
    /*
    * Draw
    */
    result = 0;
    result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
    result |= SDL_SetRenderDrawColor(renderer, r, g, b, a); 
    result |= SDL_RenderFillRect(renderer, &rect);
    return result;
}

void Primitives::DrawRoundedRectangleCairo(cairo_t* cr, double x, double y, double w, double h, double cornerRadius, SDL_Color color) {
    //double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;
    double r = (double)color.r / 255.0;
    double g = (double)color.g / 255.0;
    double b = (double)color.b / 255.0;
    double a = (double)color.a / 255.0;

    cairo_new_sub_path(cr);
    cairo_arc(cr, x + w - cornerRadius, y + cornerRadius, cornerRadius, -90 * degrees, 0 * degrees);
    cairo_arc(cr, x + w - cornerRadius, y + h - cornerRadius, cornerRadius, 0 * degrees, 90 * degrees);
    cairo_arc(cr, x + cornerRadius, y + h - cornerRadius, cornerRadius, 90 * degrees, 180 * degrees);
    cairo_arc(cr, x + cornerRadius, y + cornerRadius, cornerRadius, 180 * degrees, 270 * degrees);
    cairo_close_path(cr);

    cairo_set_source_rgba(cr, r, g, b, a);
    cairo_fill(cr);
}