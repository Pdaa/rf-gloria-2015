#pragma once

#include "ofMain.h"
#include "Defines.h"
#include "ofxSyphon.h"
#include "ofxOsc.h"
#include "ContentScene.h"
#include "LampWalker.h"
#include "QuickTrail.h"
#include "Triangles.h"
#include "PerlinWaves.h"
#include "ofxUI.h"
#include "Mapping.h"
#include "FluidScene.h"
#include "BasicParticles.h"
#include "petriDish.h"

class ofApp : public ofBaseApp {
public:

    void setup();
    void update();
    void draw();
    void debugDraw();
    void drawGrid();
    void exit();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofxOscReceiver oscReceiver;
    
    ofxOscSender * oscSenderOne;
    ofxOscSender * oscSenderTwo;
    
    ofxSyphonServer syphonOut;
    ofxSyphonClient * syphonIn;
    
    ofxSyphonServerDirectory directory;
    void serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg);
    void serverUpdated(ofxSyphonServerDirectoryEventArgs &args);
    void serverRetired(ofxSyphonServerDirectoryEventArgs &arg);
    int dirIdx;

    ofColor bg;
    ofFbo fboOut;
    Mapping * mapping;
    ofxXmlSettings XML;
    
    // gui
    void setGUI();
    ofxUITabBar *guiTabBar;
    ofxUICanvas *mainGui;
    vector<ofxUICanvas *> guis;
    void guiEvent(ofxUIEventArgs &e);
    
    bool hideGUI;
    bool drawGuide;
    bool drawSyponIn;
    bool drawOutput;
    bool outputCombined;
    bool drawMask;
    
    vector<ContentScene*> scenes;
    
};
