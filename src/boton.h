//
//  button.h
//  opencvHaarFinderExample
//
//  Created by Federico Joselevich Puiggrós on 28/09/12.
//
//
#pragma once

#include "ofMain.h"

class Boton {
public:
    void setup(int _x, int _y, string imgUp, string imgDown, bool setOnPress = true);
    void setup(int _x, int _y, int _w, int _h);
    bool isPressed();
    void render();
    void setActive(bool);
    void setSelected(bool);
    
private:
    int x;
    int y;
    int w;
    int h;
    ofImage images[2];
    bool onPress;
    bool estado;
    int upidx;
    int downidx;
};

