/*
 This code was developed by Federico Joselevich Puiggros <f@ludic.cc>
 www.ludic.cc
 
 Any modifications must include the text: "originaly developed by Federico Joselevich Puiggros, desing: 'intimidad romero'".
 
 
 
 */

#pragma once

#include "ofMain.h"
#include "ofTrueTypeFont.h"
#include "ofxCvHaarFinder.h"
#include "ofxBlurShader.h"
#include "ofxHttpUtils.h"

#include "boton.h"


#define CANT_MARCOS 5
#define INTI_VERSION 1.0


class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);		

    ofImage img;
    ofxCvHaarFinder finder;
    
    ofImage marcos[CANT_MARCOS];
    ofImage filtros[CANT_MARCOS];
    int currentMarco, currentFiltro;
    
    
    ofxCvGrayscaleImage grayImage;

    ofxBlurShader blur;
    
    float lineaExtraY;
    float lastMinY;
    float lastMaxY;

    ofVideoGrabber 		vidGrabber;
    unsigned char * 	videoInverted;
    ofTexture			videoTexture;
    int 				camWidth;
    int 				camHeight;
    unsigned int dbgtime;
    
    ofTrueTypeFont tipo;
    bool tomarProxima;
    
    /**** Interfaz ****/
    ofImage baseImg;
    ofImage logo;
    ofImage bajoShoot;
    Boton obturador;
    ofSoundPlayer sObturador;
    Boton bfiltros[4];
    Boton bmarcos[4];
    Boton bsubir;
    Boton bverEnWeb;
    
    void releaseSnap();
    
    /**** Send file ****/
    void newResponse(ofxHttpResponse & response);
    
    ofxHttpUtils httpUtils;
    char lastfile[256];
    string responseStr;
    
    void subirProxima();
    
private:
    bool showingSnap;
    bool snapCapture;
    ofImage snap;
    
    ofFbo fboCapture;
    
    string message;
};
