#include "ofApp.h"

void ofApp::setup() {
    
    ofEnableAlphaBlending();
    
    oscReceiver.setup(OSCRECEIVEPORT);

    //try {
    oscSenderOne = new ofxOscSender();
    oscSenderOne->setup(OSCCLIENTONE, OSCSENDPORT);

    oscSenderTwo = new ofxOscSender();
    oscSenderTwo->setup(OSCCLIENTTWO, OSCSENDPORT);
    
    ofSetLogLevel(OF_LOG_NOTICE);
    ofSetFrameRate(TARGET_FRAMERATE);
    ofSetVerticalSync(true);
    glEnable(GL_LINES);
    ofSetWindowTitle("Obscure Glorius Control 2014");
    
    syphonOut.setName("Gloria Main");
    
    syphonIn = new ofxSyphonClient();
    
    syphonIn->setApplicationName("Millumin");
    syphonIn->setServerName("");
    syphonIn->setup();
    
    directory.setup();
    
    //register for our directory's callbacks
    ofAddListener(directory.events.serverAnnounced, this, &ofApp::serverAnnounced);
    ofAddListener(directory.events.serverUpdated, this, &ofApp::serverUpdated);
    ofAddListener(directory.events.serverRetired, this, &ofApp::serverRetired);
    dirIdx = -1;
    
    mapping = new Mapping();
    mapping->load("mapping.xml", "input1.svg");
    
    // effects scenes
    
    // Set up the scenes, all scenes is a subclass of SceneContent, don't call draw, setup and update directly it is taken care of thorugh the scene.
    
    scenes.push_back(new FluidScene());
    //transformer = new Transformer();
    //scenes.push_back(transformer);
    scenes.push_back(new QuickTrail());
    scenes.push_back(new Triangles());
    scenes.push_back(new PerlinWaves());
    scenes.push_back(new BasicParticles());
    //scenes.push_back(new PetriDish());
    
    ofFbo::Settings fboSettings;
    fboSettings.height = OUTHEIGHT;
    fboSettings.width  = OUTWIDTH;
    fboSettings.numSamples = 4;
    fboSettings.useDepth   = false;
    
    fboOut.allocate(fboSettings);
    
    fboOut.setUseTexture(true);
    
    fboOut.begin();
    ofBackground(0,0,0,255);
    fboOut.end();
    
        
    for(int i=0; i<scenes.size(); i++) {
        scenes[i]->mapping  = mapping;
        scenes[i]->syphonIn = syphonIn;
        scenes[i]->oscClients.push_back(oscSenderOne);
        scenes[i]->oscClients.push_back(oscSenderTwo);
        scenes[i]->setupScene(OUTWIDTH, OUTHEIGHT, i);
    }
    
    setGUI();
}

void ofApp::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Announced")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

void ofApp::serverUpdated(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Updated")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

void ofApp::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Retired")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    dirIdx = 0;
}

//--------------------------------------------------------------
void ofApp::update() {
    
    while(oscReceiver.hasWaitingMessages()){
        
		// get the next message
		ofxOscMessage m;
		oscReceiver.getNextMessage(&m);

        //cout<<m.getAddress()<<endl;
        for(int i=0; i<scenes.size();i++) {
            scenes[i]->parseSceneOscMessage(&m);
        }
    }

    // Scenes
    for(int i=0; i<scenes.size(); i++) {
        scenes[i]->updateScene();
    }
    
    // OSC in listen
}

void ofApp::draw() {
    
    // Draw scene fbo's
    ofPushStyle();
    ofNoFill();
    for(int i=0; i<scenes.size(); i++) {
        ofSetColor(255);
        ofFill();
        ofPushStyle();
        scenes[i]->drawScene();
        ofPopStyle();
    }
    ofPopStyle();
    
   ofPushStyle();{
        fboOut.begin();{
            ofEnableAlphaBlending();
            ofClear(0, 0);
            
            for(int i=0; i<scenes.size(); i++) {
                ofSetColor(255,255,255,scenes[i]->opacity*255);
                
                if(scenes[i]->enabled) {
                    scenes[i]->fbo.draw(0,0);
                }
            }
            
            //glBlendFunc(GL_FUNC_SUBTRACT​);
            if(drawMask) mapping->drawMask();
            
        }fboOut.end();
    } ofPopStyle();
    
    ofDisableDepthTest();
    ofBackground(0, 0, 0);
    ofSetColor(255,255,255,255);

    float scale = 0.08;
    ofPushMatrix();{
        ofTranslate(ofGetWidth()-scale*fboOut.getWidth()-40, 40);
        
        //ofScale(0.08, 0.08);
        
        ofSetColor(255,255,255,255);
        ofNoFill();
        ofSetLineWidth(1);
        
        for(int i=0; i<scenes.size(); i++) {
            if(scenes[i]->enabled){
                ofSetColor(255);
            } else {
                ofSetColor(255,0,0,100);
            }

            ofDrawRectangle(-1, -1, scale*fboOut.getWidth()+2, scale*fboOut.getHeight()+2);
           // fboOut.draw(0, 0);
            ofSetColor(255,255,255,scenes[i]->opacity*255);
            
            if(scenes[i]->enabled) {
                scenes[i]->fbo.draw(0,0, scenes[i]->fbo.getWidth()*scale, scenes[i]->fbo.getHeight()*scale);
            }
            
            ofSetColor(255);
            
            ofDrawBitmapString(scenes[i]->name + "    ("+ofToString(scenes[i]->opacity*100.,0)+"%)", ofPoint(0,-3));

            if(drawGuide) {
                //ofSetColor(255,255,255,96);
                ofPushMatrix();
                ofScale(scale, scale);
                drawGrid();
                debugDraw();
                ofPopMatrix();
            }
            
            ofTranslate(0, scale*fboOut.getHeight()+30);
        }
        
    
        ofTranslate(0, 30);
        //Syphon
        ofPushMatrix();
    
        ofSetColor(0,0,255);
        ofSetLineWidth(1);
        ofDrawRectangle(-1, -1, scale*syphonIn->getWidth()+2, scale*syphonIn->getHeight()+2);
        
        ofSetColor(255);
        ofDrawBitmapString("Syphon input - (Press 'i' to change)",  ofPoint(0,-18));
        ofDrawBitmapString(syphonIn->getApplicationName()+" "+syphonIn->getServerName(),  ofPoint(0,-3));

        syphonIn->draw(0, 0, scale*syphonIn->getWidth(), scale*syphonIn->getHeight());
        
        ofPopMatrix();
        
    }ofPopMatrix();
   
    /*ofPushMatrix();{
        ofTranslate(80, 520);
        
        if(syphonIn->isSetup()){
            
            ofPushMatrix();
            
            ofSetColor(255);
            ofSetLineWidth(1);
            ofRect(-1, -1, scale*syphonIn->getWidth()+2, scale*syphonIn->getHeight()+2);
            syphonIn->draw(0, 0, scale*syphonIn->getWidth(), scale*syphonIn->getHeight());
            
            ofPopMatrix();
            
            ofDrawBitmapString("Syphon input - (Press 'i' to change)", 10,18);
            ofDrawBitmapString(syphonIn->getApplicationName()+" "+syphonIn->getServerName(), 10,34);
        }
    }ofPopMatrix();*/
    
    ofSetColor(255);
    ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), ofGetWidth()-200, 20);
    
    if(mapping->selectedCorner) {
        ofDrawBitmapString("Selected Corner: " + ofToString(mapping->selectedCorner->uid) + " pos: " + ofToString(mapping->selectedCorner->pos), ofGetWidth()-600, 20);
    }
    
    for(int i=0; i<scenes.size(); i++) {
        if(scenes[i]->enabled) {
            scenes[i]->publishSyphonTexture();
        }
    }
    
    //syphonOut.publishTexture(&fboOut.getTexture());
}

//------------------------------------------------------------
void ofApp::debugDraw() {
    mapping->debugDraw();
}

void ofApp::drawGrid() {
    ofSetLineWidth(1);
    for(int i =0; i<mapping->triangles.size();i++) {
        mapping->triangles[i]->mesh.drawWireframe();
    }
}

void ofApp::setGUI()
{
    
    float dim = 16;
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float width = 255-xInit;
    hideGUI = false;
    
    guiTabBar = new ofxUITabBar();
    mainGui = new ofxUICanvas();
    
    mainGui->setFont("GUI/Arial.ttf");
    mainGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    guiTabBar->setFont("GUI/Arial.ttf");
    guiTabBar->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    mainGui->addLabel("Gloria");
    mainGui->addLabel("OSC info");
    mainGui->addLabel("In: " + ofToString(OSCRECEIVEPORT));
    mainGui->addLabel("Out: " + string(OSCCLIENTONE) + " & " + string(OSCCLIENTTWO) + ":" + ofToString(OSCSENDPORT));
    
    mainGui->addToggle("Draw guide", &drawGuide);
    mainGui->addToggle("Draw mask", &drawMask);
    mainGui->autoSizeToFitWidgets();
    ofAddListener(mainGui->newGUIEvent,this,&ofApp::guiEvent);
    
    for(int i=0; i<scenes.size(); i++) {
        scenes[i]->setSceneGui();
        guiTabBar->addCanvas(scenes[i]->gui);
        scenes[i]->gui->setColorBack(ofColor(0,100 + 20*i,0,255));
        guis.push_back(scenes[i]->gui);
    }
    
    guiTabBar->autoSizeToFitWidgets();
    ofAddListener(guiTabBar->newGUIEvent,this,&ofApp::guiEvent);
    
    guiTabBar->setPosition(0, mainGui->getRect()->height+10);
    //guiTabBar->setScrollAreaToScreenHeight();
    
    mainGui->setColorBack(ofColor(0,150,200,255));
    guiTabBar->setColorBack(ofColor(0,100,0,255));
    
    mainGui->loadSettings("GUI/guiMainSettings.xml");
    guiTabBar->loadSettings("GUI/guiSettings.xml", "ui-");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(key == 'i') {
        dirIdx++;
        if(dirIdx > directory.size() - 1)
        dirIdx = 0;
        
        if(directory.isValidIndex(dirIdx)){
            syphonIn->setServerName(directory.getServerList()[dirIdx].serverName);
            syphonIn->setApplicationName(directory.getServerList()[dirIdx].appName);
        }
    }
    
    if(key == 'n') {
        mapping->nextCorner();
    }
    
    if(key == 'm') {
        mapping->prevCorner();
    }
    
    if(mapping->selectedCorner) {
        if(key == OF_KEY_UP) {
            mapping->selectedCorner->pos.z += 1;
            mapping->updateMeshes();
        }
    
        if(key == OF_KEY_DOWN) {
            mapping->selectedCorner->pos.z -= 1;
            mapping->updateMeshes();
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
}

//--------------------------------------------------------------
void ofApp::exit()
{
    mainGui->saveSettings("GUI/guiMainSettings.xml");
    guiTabBar->saveSettings("GUI/guiSettings.xml", "ui-");
    mapping->save();
    
    delete guiTabBar;
    delete mapping;
}

void ofApp::guiEvent(ofxUIEventArgs &e)
{
}
