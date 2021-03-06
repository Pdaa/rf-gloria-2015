//
//  TriMasker.cpp
//  Gloria
//
//  Created by Johan Bichel Lindegaard on 03/07/2013.
//
//

#include "TriBlobs.h"

void TriBlobs::setup(){
    
    name = "Triangle Blobs";
    oscAddress = "/triblobs";
    
}

void TriBlobs::update(){
    
}

void TriBlobs::draw(){;
    
    float speedV = ofMap(speed, 0, 1, -2, 2);
    
    ofSetLineWidth(6);
    // waves going across red
    for(int i =0; i<mapping->triangles.size();i++) {
        ofSetColor( 255, 255, 255, 255);
	mapping->triangles[i]->polyline.getResampledBySpacing(90).getSmoothed(ofNoise(mapping->triangles[i]->centroid.y/600 - ofGetElapsedTimef()*speed)*smoothness).draw();
        
        //ofSetColor( 255, 255, 255, ofNoise(mapping->triangles[i]->centroid.x/600 - ofGetElapsedTimef()/speed) *255 );
        //mapping->triangles[i]->polyline.draw();
    }
    
}


void TriBlobs::parseOscMessage(ofxOscMessage *m){
    ContentScene::parseOscMessage(m);
    
 	vector<string> adrSplit = ofSplitString(m->getAddress(), "/");
	string rest = ofSplitString(m->getAddress(), "/"+adrSplit[1])[1];
    
	if(adrSplit[1] == "scene"+ofToString(index) || "/"+adrSplit[1] == oscAddress) {
    
        
        if(rest == "/smoothness/x" ) {
            smoothness = m->getArgAsInt32(0);
	    } 
        
    }
}

void TriBlobs::setGui(){
    
    //ContentScene::setGui(gui, width);
    
    string i = "["+ ofToString(index) + "] ";
    
    
    gui->addSlider(i+"smoothness", 0, 30, &smoothness);
    
    gui->addToggle(i+"animate", &animate);
    
    /*gui->addSlider(i+"Y Speed", minSpeed, maxSpeed, &ySpeed);
    
    gui->addSlider(i+"X Scatter", 0, 6000, &xScatter);
    gui->addSlider(i+"Y Scatter", 0, 6000, &yScatter);
    
    gui->addToggle(i+"Reverse", &reverse);
    gui->addToggle(i+"Crazy speed", &crazySpeed);
    */
    
    
    
}