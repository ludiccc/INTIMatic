/*
 This code was developed by Federico Joselevich Puiggros <f@ludic.cc>
 www.ludic.cc
 
 Any modifications must include the text: "originaly developed by Federico Joselevich Puiggros, desing: 'intimidad romero'".
 
 
 
 */

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	camWidth 		= 640;	// try to grab at this size.
	camHeight 		= 480;
	
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(camWidth,camHeight);
	
    img.allocate(camWidth,camHeight, OF_IMAGE_COLOR);
	//img.loadImage("test.jpg");
    //img..... hay que traer la info de la textura
	finder.setup("haarcascade_frontalface_alt.xml");
    finder.findHaarObjects(img);
    blur.setup(680,480);
    
    tipo.loadFont("XG-pixo.TTF",8);
    
    marcos[0].loadImage("filtros/MarcoRedonBlanco.png");
    marcos[1].loadImage("filtros/MarcoRedonNegro.png");
    filtros[0].loadImage("filtros/map-5.png");
    filtros[1].loadImage("filtros/map-2.png");
    filtros[2].loadImage("filtros/map-1.png");
    filtros[3].loadImage("filtros/map-5.png");
        
    currentMarco = 2;
    currentFiltro = 3;
    ofEnableAlphaBlending();
    tomarProxima = false;
    

    sObturador.loadSound("72714__horsthorstensen__shutter-photo.wav");
    sObturador.setLoop(false);
    
    baseImg.loadImage("base_app_big.png");
    bajoShoot.loadImage("bajo-shoot.png");
    obturador.setup(518,612+6,"obturador-up.jpg", "obturador-down.jpg");
    bsubir.setup(23,612+17,"upload-down.png", "upload-up.png");
    //bverEnWeb.setup(23,612,30,10);
    //bsubir.setActive(false);
    
    for (int i = 0; i < 3; i++) {
        bfiltros[i].setup(75+i*26, 612+26, "filtro-act.png", "filtro-desact.png");
    }
    bfiltros[3].setup(75+3*26, 612+26, "marco-sin-act.png", "FiltroDESactivadoDEF.gif");
    bfiltros[3].setSelected(true);
    bmarcos[0].setup(186, 612+26, "marco-act.png", "marco-desact.png");
    bmarcos[1].setup(212, 612+26, "marco-act.png", "marco-desact.png");
    bmarcos[2].setup(240, 612+26, "marco-sin-act.png", "marco-sin-desact.png");
    bmarcos[2].setSelected(true);

	ofAddListener(httpUtils.newResponseEvent,this,&testApp::newResponse);
	httpUtils.start();
    httpUtils.setVerbose(true);
    sprintf(lastfile, "");
    
    snapCapture = false;
    message.clear();
    lineaExtraY = -1;
    lastMinY = 300;
    
    fboCapture.allocate(612,612);
    fboCapture.begin();
	ofClear(0,0,0);
    fboCapture.end();
    ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2,(ofGetScreenHeight()-ofGetHeight())/2);
}

//--------------------------------------------------------------
void testApp::update(){
    dbgtime = ofGetElapsedTimeMillis();
	vidGrabber.update();
	if (vidGrabber.isFrameNew()){
        
        ofxCvColorImage colorImg;
        colorImg.allocate(camWidth,camHeight);
        colorImg.setFromPixels(vidGrabber.getPixelsRef());
        grayImage = colorImg;
        /*
		int totalPixels = camWidth*camHeight*3;
		unsigned char * pixels = vidGrabber.getPixels();
        unsigned char * pixelsDest = img.getPixels();
		for (int i = 0; i < totalPixels; i++){
			pixelsDest[i] = pixels[i];
		}*/
        finder.findHaarObjects(grayImage, 80, 80);
	}

}

//--------------------------------------------------------------
void testApp::draw(){
    int minY = 612;
    int maxY = 0;

    ofSetColor(255);
    ofBackground(0);
    if (snapCapture) {
        //snap.draw(0,0);
    } else {
        fboCapture.begin();
        ofClear(0,0,0);
        ofPushMatrix();
        ofScale(612/(float)vidGrabber.height, 612/(float)vidGrabber.height);
        ofTranslate(612/2-vidGrabber.width/2, 0);
        unsigned char * pixels = vidGrabber.getPixels();
        blur.begin();
        blur.amount = 1; //ofMap(mouseX,0,ofGetWidth(),0,10,true);
        blur.iterations = 1; //ofMap(mouseY,0,ofGetHeight(),1,10,true);
        //vidGrabber.draw(0, 0);
        for (int x = 0; x < vidGrabber.width; x++)
            for (int y = 0; y <vidGrabber.height; y++) {
                ofColor c(pixels[(y*vidGrabber.width+x)*3],
                           pixels[(y*vidGrabber.width+x)*3+1],
                           pixels[(y*vidGrabber.width+x)*3+2]);
                if (currentFiltro == 0) {
                        float hue, saturation, brightness;
                        c.getHsb( hue, saturation, brightness );
                        c.setSaturation(saturation * 1.2);
                }
                ofSetColor(c.r, c.g, c.b);
                ofRect(x,y,1,1);
            }
        
        blur.end();
        
        blur.begin();
        blur.amount = 1.8; //ofMap(mouseX,0,ofGetWidth(),0,10,true);
        blur.iterations = 1; //ofMap(mouseY,0,ofGetHeight(),1,10,true);
        ofNoFill();
        
        
        if (lineaExtraY > -1) {
            minY = lineaExtraY - 20;
            maxY = lineaExtraY + 20;
        }
        for(int i = 0; i < finder.blobs.size(); i++) {
            ofRectangle cur = finder.blobs[i].boundingRect;
            if (cur.y < minY) minY = cur.y;
            if (cur.y+cur.height > maxY) maxY = cur.y+cur.height;
        }

        if (minY == 612) minY = lastMinY;
        if (maxY < minY) {
            if (minY == 300) maxY = minY + 80;
            else {
                maxY = lastMaxY;
            }
            
        }
        lastMinY = minY;
        lastMaxY = maxY;
        
        ofFill();
        float sqSize = (maxY-minY)/4;
        for (int x = 0; x < vidGrabber.width; x+= sqSize)
            for (int y = minY; y < maxY-sqSize*2; y+=sqSize) {
                ofSetColor(pixels[(y*vidGrabber.width+x)*3],
                           pixels[(y*vidGrabber.width+x)*3+1],
                           pixels[(y*vidGrabber.width+x)*3+2]);
                ofRect(x,y,sqSize,MIN(sqSize,maxY-y));
            }
        ofSetColor(255,255,255);
        blur.end();
        ofPopMatrix();
        fboCapture.end();
    }
    fboCapture.draw(0,0);
    
        
    ofSetColor(255);
    /*
     if (currentFiltro > -1) for (int i = 0; i < height; i+=filtros[currentMarco].height) blend(filtros[currentMarco],0,0,filtros[currentMarco].width,filtros[currentMarco].height,0,i,width,filtros[currentMarco].height,ADD);
     

     */
    ofEnableAlphaBlending();
    if (currentFiltro < 3)
    {
        
        filtros[currentFiltro].draw(0,0,612,612);

        /* ADD Blend  output_color = 1 * source_color + 1 * destination_color*/
        /*glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_ONE);

        for (int i = 0; i < 612; i+= filtros[currentFiltro].height)
        {
            //for (int j = 0; j < filtros[currentMarco].height; j++) {
                filtros[currentFiltro].draw(0,i,612,filtros[currentMarco].height);
            //}
        }
        glDisable(GL_BLEND);*/
    }
    
    if (currentMarco < 2) marcos[currentMarco].draw(0,0,612,612);
    
    
    /**** interfaz ****/
    ofSetColor(255);
    baseImg.draw(0,612);
    obturador.render();
    bsubir.render();
    //bverEnWeb.render();
    for (int i = 0; i < 3; i++) bmarcos[i].render();
    for (int i = 0; i < 4; i++) bfiltros[i].render();

    if (tomarProxima) {
        sprintf(lastfile, "INTIMatic-%04i-%02i-%02i-%02i-%02i-%02i-.jpg", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds());
        ofPixels pixels;
        snap.grabScreen(0,0,612,612);
        snap.saveImage(lastfile);
        message.clear();
        releaseSnap();
        
        subirProxima();
        
        tomarProxima = false;
    }

    char msj[256];
    sprintf(msj, "v. %1.2f", INTI_VERSION);
    ofSetColor(164,164,164);
    tipo.drawString(msj, 500, 750);

    if (!message.empty()) {
        ofSetColor(0,0,0,50);
        ofRect(0,612-30, ofGetWidth(), 30);
        ofSetColor(164);
        tipo.drawString(message, 30, 612-13);
    }

    
    ofSetColor(255);
    
    if (logo.bAllocated()) logo.draw(ofGetWidth()-logo.width-20, 612-logo.height-20);

    ofSetColor(0);
    ofRect(612,0,ofGetWidth(),ofGetHeight());
    ofTranslate(672, ofGetHeight()-30);
    ofRotate(-90, 0, 0, 1);

    
    ofSetColor(255);    
    tipo.drawString("Usted no deber’a ver este mensaje pero, ya que lo ve vamos a charlar un ratito", 0, 0);
    tipo.drawString("En las Žpocas en las que estamos, en donde lo pœblico y lo privado son relaciones",0, 15);
    tipo.drawString("con una tensi—n diferente a la que la humanidad tend’a a proponer...", 0, 30);
}

//--------------------------------------------------------------
void testApp::subirProxima() {
    if (!strcmp(lastfile, "")) return;
	ofxHttpForm form;
	form.action = "http://www.intimatic.com.ar/cgi-bin/upload.cgi";
	form.method = OFX_HTTP_POST;
	form.addFormField("name", "intimatic");
	form.addFile("photo",lastfile);
	httpUtils.addForm(form);
    cout << "[subirProxima] enviando foto a " << form.action << std::endl;
    message = "Sending image...";
    
}
//--------------------------------------------------------------
void testApp::newResponse(ofxHttpResponse & response){
	responseStr = (string)response.responseBody;
    cout << "[newResponse] " << responseStr << std::endl;
    message = responseStr;
    
    if (responseStr.find("http://www.intimatic.com.ar/") != string::npos) {
        
        
    #ifdef __linux
        std::string browser = getenv("BROWSER");
        if(browser == "") return;
        
        char *args[3];
        args[0] = (char*)browser.c_str();
        args[1] = (char*)responseStr.c_str();
        args[2] = 0;
        
        pid_t pid = fork();
        if(!pid)
            execvp(browser.c_str(), args);
    }

    #endif
    #ifdef _WIN32
        string urlToSend;
        urlToSend = "" + responseStr;
        ShellExecute(NULL, "open", urlToSend.c_str(),
                     NULL, NULL, SW_SHOWNORMAL);
    #endif
    #ifdef __APPLE_CC__
        string urlToSend;
        urlToSend = "open " + responseStr;
        system(urlToSend.c_str());
    #endif
        
        
    }
    
    /* para windows:
    ShellExecute(NULL, "open", "http://google.net",
                 NULL, NULL, SW_SHOWNORMAL);
     */
    
    /*
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString: [[[[NSString alloc] initWithCString: url.c_str()]stringByAddingPercentEscapesUsingEncoding: NSASCIIStringEncoding] autorelease] ]];
     */
    /* 
     Aqu’, recibimos una respuesta: Žste c—digo es para UNIX para abrir un browser con la p‡gina:
     
     #include <unistd.h>
     #include <stdlib.h>
     
     void launch(const std::string &url)
     {
     std::string browser = getenv("BROWSER");
     if(browser == "") return;
     
     char *args[3];
     args[0] = (char*)browser.c_str();
     args[1] = (char*)url.c_str();
     args[2] = 0;
     
     pid_t pid = fork();
     if(!pid)
     execvp(browser.c_str(), args);
     }
     
     este es para OSX:
     system("open http://www.apple.com");
     
     Y este es para Windows:
     #include <windows.h>
     
     void main()
     {
     ShellExecute(NULL, "open", "http://yourwebpage.com",
     NULL, NULL, SW_SHOWNORMAL);
     }
     */
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if (key == ' ')
    {
        bsubir.setSelected(true);
        sObturador.play();
        message = "Press upload button to send image. Press over the image to cancel.";
        snapCapture = true;
    }
}
//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

void testApp::releaseSnap() {
    snapCapture = false;
    //bsubir.setActive(false);
    bsubir.setSelected(false);
    message.clear();
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    /* Si hago click sobre la imagen, genera una l’nea de pixelado en ese lugar. Al hacer click de nuevo se va */
    if (y < 612) {
        if (snapCapture) {
            releaseSnap();
        }
        else lineaExtraY = (lineaExtraY>-1)?-1:y*(612/(float)vidGrabber.height);
        return;
    }
    
    if (obturador.isPressed()) {
        bsubir.setSelected(true);
        sObturador.play();
        message = "Press upload button to send image. Press over the image to cancel.";
        snapCapture = true;
    }
        
    for (int i = 0; i < 4; i++) {
        if (bfiltros[i].isPressed()) {
            currentFiltro = i;
            for (int j = 0; j < 4; j++) {
                bfiltros[j].setSelected(false);
            }
            
            bfiltros[currentFiltro].setSelected(true);
            break;
        }
    }
    for (int i = 0; i < 3; i++) {
        if (bmarcos[i].isPressed()) {
            currentMarco = i;
            for (int j = 0; j < 3; j++) {
                bmarcos[j].setSelected(false);
            }
            bmarcos[currentMarco].setSelected(true);
            break;
        }
    }

    if (bsubir.isPressed()) {
        tomarProxima = true;
    }
    
    /*if (bverEnWeb.isPressed()) {
        //NSURL *url = [NSURL URLWithString:@"http://www.google.com/"];
        //[[UIApplication sharedApplication] openURL: url];
        //[[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://www.apple.com"]];
        

    }*/
}

//--------------------------------------------------------------

void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    //ofSetWindowShape(612,683);
    if (h < 797) {
        baseImg.loadImage("base_app.png");
        logo.loadImage("logo.png");
        h = 683;
    } else {
        h = 797;
    }
    ofSetWindowShape(612, h);
    cout << "Window resized " << w << " " << h << std::endl;
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}