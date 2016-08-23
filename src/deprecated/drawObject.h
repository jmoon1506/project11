#ifndef DRAWOBJ_H
#define DRAWOBJ_H

#include <string>

class DrawObject {
 public:
    virtual void Render() = 0;
};

class Text : DrawObject {
 public:
    string content;
    void Render();
};

class Image : DrawObject {

};

#endif