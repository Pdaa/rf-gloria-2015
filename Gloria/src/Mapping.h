//
//  mapping.h
//  gloriamap
//
//  Created by Jonas Jongejan on 30/06/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxSvg.h"
#include "ofxXmlSettings.h"

struct InputTriangle;

class Corner {
public:
    int uid;
    vector<InputTriangle*> triangles;
    vector<Corner*> joinedCorners;
    ofVec3f pos;
    //todo: ofVec3f worldPos; for using with real lamp posisitons
    void addTriangleReference(InputTriangle* triangle);
};

class InputTriangle {
public:
    int uid;
    void debugDraw();
    
    ofPoint centroid;
    ofPolyline polyline;
    
    Corner * corners[3];
    //ofVec3f center;
    ofMesh mesh;
    ofColor color;
    
    InputTriangle();
    ~InputTriangle();
    
    ofVec3f getCenter();
};

class Mapping {	
public:
    Mapping() {};
    virtual ~Mapping() {};
    
    vector<InputTriangle*> triangles;
    vector<Corner*> corners;
    
    void load(string xmlfile, string svgfile);
    void generate();
    void save();
    void exit();
    void debugDraw();
    
    //InputTriangle * selectedTriangle;
    Corner * selectedCorner;
    int selectedCornerId = 0;
    
    void nextCorner();
    void prevCorner();
    void selectCornerAt(int x, int y, int r);
    
protected:
    string svgFilename;
    string xmlFilename;
    ofxXmlSettings settings;
    ofxSVG svg;
};

