/*
 
 INTIMatic por Federico Joselevich Puiggros e "Intimidad Romero" se encuentra bajo una Licencia Creative Commons Atribuci—n 3.0 Unported.
 
 
 This code was developed by Federico Joselevich Puiggros <f@ludic.cc>
 www.ludic.cc
 
 Any modifications must include the text: "originaly developed by Federico Joselevich Puiggros, desing: 'intimidad romero'".
 
 
 
 */

#include "ofAppGlutWindow.h"
#include "testApp.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 612, 797, OF_WINDOW);
	ofRunApp(new testApp());
}


