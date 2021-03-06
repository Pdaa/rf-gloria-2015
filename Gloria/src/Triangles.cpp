//
//  Triangles.cpp
//  Gloria
//
//  Created by Jonas Jongejan on 01/07/13.
//
//

#include "Triangles.h"


void Triangles::setGui(){
    gui->addSlider("/SyphonOpacity/x", 0,1, &syphonOpacity);
    
    gui->addSlider("/DivideTriangleSize/x", 0,5, &divideTriangleSize);
    
    gui->addSlider("/DivideRadius/x", 0,5100, &divideRadius);
    gui->addToggle("/DivideInvert/x", &divideInvert);
    //gui->addSlider("TransitionTime", 0,10, &transitionTime);
    
    
    
    gui->addSlider("/light/x", 0,1, &light);
    gui->addSlider("/LightSpeed/x", 0,1, &lightSpeed);
    
    //gui->addSlider("Direct Opactiry", 0,1, &directTextureOpacity);
    
    gui->addSlider("/ColorR/x", 0,1, &colorR);
    gui->addSlider("/ColorG/x", 0,1, &colorG);
    gui->addSlider("/ColorB/x", 0,1, &colorB);

    gui->addSlider("/Wireframe/x", 0,1, &wireframeAlpha);
    gui->addSlider("/Fill/x", 0,1, &fillAlpha);

    gui->addSlider("/Noise/x", 0,2, &noise);
    gui->addSlider("/NoiseSeed/x", 0,1, &noiseSeed);
    gui->addSlider("/NoiseSeedSpeed/x", 0,1, &noiseSeedSpeed);

}


void Triangles::setup(){
    name = "Triangles";
    oscAddress = "/tri";
    
    map<Corner*, Corner*> cornerRefs;
    
    for(int i=0;i<mapping->triangles.size();i++){
        InputTriangle * triangle = mapping->triangles[i];
        
        SubTriangle * subTriangle =  new SubTriangle();
        subTriangle->level = 0;
        
        //Create a duplicate of the corners
        for(int j=0;j<3;j++){
            if(cornerRefs.find(triangle->corners[j]) != cornerRefs.end()){
                subTriangle->corners[j] = cornerRefs[triangle->corners[j]];
            } else {
                subTriangle->corners[j] = new Corner();
                subTriangle->corners[j]->pos = triangle->corners[j]->pos;
                subTriangle->corners[j]->origPos = triangle->corners[j]->pos;
               // subTriangle->corners[j]->randomSeed = ofVec3f(0,0,ofRandom(-0.5,0.5));
                
                
                cornerRefs[triangle->corners[j]] = subTriangle->corners[j];

            }
        }
        
        subTriangle->parentTriangle = subTriangle;
        subTriangle->age = 1000;
        
        subTriangles[mapping->triangles[i]] =  subTriangle ;
        
        center = ofVec2f(1500,500);
        divideRadius = 0;
    }
    
    
    //Duplicate the joined corners
    for(int i=0;i<mapping->triangles.size();i++){
        InputTriangle * triangle = mapping->triangles[i];
        SubTriangle * subTriangle = subTriangles[triangle];
        
        for(int j=0;j<3;j++){
            for(int k=0; k<triangle->corners[j]->joinedCorners.size(); k++){
                Corner * c1 = triangle->corners[j]->joinedCorners[k];
                Corner * c2 = cornerRefs[c1];
                subTriangle->corners[j]->joinedCorners.push_back(c2);
            }
        }
    }
    
    //Create division Corners
    for(int i=0;i<mapping->triangles.size();i++){
        InputTriangle * triangle = mapping->triangles[i];
        SubTriangle * subTriangle = subTriangles[triangle];
        
        for(int j=0;j<3;j++){
            subTriangle->corners[j]->createDivisionCorners();
        }
        
    }
    
    //Divide
    for(int i=0;i<mapping->triangles.size();i++){
        InputTriangle * triangle = mapping->triangles[i];
        SubTriangle * subTriangle = subTriangles[triangle];


        subTriangle->divide(38000,1);
        
    }
    
    
    //
    
    for(int it=0;it<2;it++){
        for(int i=0;i<mapping->triangles.size();i++){
            InputTriangle * triangle = mapping->triangles[i];
            SubTriangle * subTriangle = subTriangles[triangle];
            vector<SubTriangle*> arr = subTriangle->getLowestLevelTriangles();
            
            for(int j=0;j<arr.size();j++){
                SubTriangle * _subTriangle = arr[j];
                
                for(int k=0;k<3;k++){
                    _subTriangle->corners[k]->createDivisionCorners();
                }
                
                _subTriangle->divide(0,it+2);
                
                
            }
        }
    }

    
    pointLight.setDiffuseColor( ofColor(255.f, 255.f, 255.f));
    pointLight.setAttenuation(2.);


    
    depthFbo.allocate(OUTWIDTH, OUTHEIGHT, GL_RGB);
    
    debugShader.load("shaders/TrianglesDebug");
}




void Triangles::divide(SubTriangle * triangle, float sizeGoal){
    
}

void Triangles::collapse(SubTriangle * triangle){
   /* int subsubtriangles = -1;
    for(int i=0;i<triangle->subTriangles.size();i++){
        if(triangle->subTriangles[i]->subTriangles.size()){
            collapse(triangle->subTriangles[i]);
            subsubtriangles = i;
            //      break;
        }
    }
    
    if(subsubtriangles != -1){
    } else {
        float highestAge = 0;
        for(int i=0;i<triangle->subTriangles.size();i++){
            if(triangle->subTriangles[i]->age > transitionTime){
                triangle->subTriangles[i]->age = transitionTime;
            }
            if(triangle->subTriangles[i]->age > 0){
                triangle->subTriangles[i]->age -= triangle->ageDifference* 2 * 1.0/ofGetFrameRate();
            }
            
            if(highestAge < triangle->subTriangles[i]->age){
                highestAge = triangle->subTriangles[i]->age;
            }
        }
        
        if(highestAge <= 0){
            for(int i=0;i<triangle->subTriangles.size();i++){
                
                if(triangle->subTriangles[i]->age <= 0){
                    delete triangle->subTriangles[i];
                }
            }
            triangle->subTriangles.clear();
        }
    }*/
}


float ease(float t, float b, float c, float d) {
	t /= d/2;
	if (t < 1) return c/2*t*t + b;
	t--;
	return -c/2 * (t*(t-2) - 1) + b;
};


void Triangles::drawTriangleWireframe(SubTriangle * triangle){
    bool subDraw = false;
    for(int i=0;i<triangle->subTriangles.size();i++){
        if(triangle->subTriangles[i]->drawLevel-1.5*triangle->subTriangles[i]->ageDifference  >= triangle->subTriangles[i]->level){
            subDraw = true;
        }
    }
  
    if(subDraw){
        for(int j=0;j<triangle->subTriangles.size();j++){
            drawTriangleWireframe(triangle->subTriangles[j]);
        }
    } else {
          for(int u=0;u<3;u++){
              ofVec3f pos = triangle->getPos(u) ;
              ofVec3f center = triangle->getCenter();

              glTexCoord2d(syphonIn->getWidth()* center.x/OUTWIDTH
                           , syphonIn->getHeight()*(OUTHEIGHT-center.y)/OUTHEIGHT);

            glVertex3d(pos.x, pos.y, 0/*triangle->corners[u]->pos.z*/);
        }

    }
}




void Triangles::drawTriangle(SubTriangle * triangle, float opacity, ofVec3f parentNormal){
    ofVec3f normal = triangle->normal();

    bool subDraw = false;
    for(int i=0;i<triangle->subTriangles.size();i++){
        if(triangle->subTriangles[i]->drawLevel-1.5*triangle->subTriangles[i]->ageDifference  >= triangle->subTriangles[i]->level){
            subDraw = true;
        }
    }
    if(subDraw){
        for(int j=0;j<triangle->subTriangles.size();j++){
            drawTriangle(triangle->subTriangles[j], opacity, normal);
        }
    } else {
        
        ofVec3f ambient = ofVec3f(10);
        
        float a = triangle->drawLevel-1.5*triangle->ageDifference - triangle->level;
        float aaa = MIN(1,MAX(0,a));
        
        ofVec3f center = triangle->getCenter();
        
        ofVec3f n = normal * aaa + parentNormal * (1.0-aaa);
        
        ofVec3f trianglePos = center;
        ofVec3f lightDir = ( trianglePos- lightPos);
    
        //Tegn billede 1 pixel pr trekant
        for(int u=0;u<3;u++){
            glNormal3f(n.x, n.y, n.z);
            
            glTexCoord2d(syphonIn->getWidth()* center.x/OUTWIDTH
                         , syphonIn->getHeight()*(OUTHEIGHT-center.y)/OUTHEIGHT);
            
            ofVec3f pos = triangle->getPos(u) ;
            glVertex3d(pos.x, pos.y, 0/*pos.z*/);
        }
        

        
        //Tegn billede 1:1
/*        float bbb = directTextureOpacity;
        if(bbb){
            
            ofSetColor(255,255,255,255*bbb);
            for(int u=0;u<3;u++){
              //  glNormal3f(_normal.x, _normal.y, _normal.z);

                glTexCoord2d(syphonIn->getWidth()* triangle->corners[u]->pos.x/OUTWIDTH
                             ,syphonIn->getHeight()*(OUTHEIGHT-triangle->corners[u]->pos.y)/OUTHEIGHT);
                
                glVertex3d(triangle->corners[u]->pos.x, triangle->corners[u]->pos.y, triangle->corners[u]->pos.z);
            }
        }*/
    }
}

void Triangles::draw(){
    ofClear(0);
    ofSetColor(255,255,255);
   /*
    depthFbo.begin();{
        ofFill();
        ofClear(255);
        ofSetColor(255,255,255);
        syphonIn->draw(0, 0, OUTWIDTH, OUTHEIGHT);
    }depthFbo.end();
 */
    
    if(fillAlpha > 0){
        ofSetColor(255*colorR,255*colorG,255*colorB, 255*fillAlpha);
        
        debugShader.begin();
        // debugShader.setUniformTexture("depthTex", depthFbo.getTextureReference(), 1);
        debugShader.setUniform1f("lightAmount", light);
        debugShader.setUniform1f("textureAmount", syphonOpacity);
        syphonIn->bind();
        material.setShininess(15);
        
        
        
        ofEnableLighting();
        pointLight.enable();
        material.begin();
        
        
        glBegin(GL_TRIANGLES);
        for(int i=0;i<mapping->triangles.size();i++){
            drawTriangle(subTriangles[mapping->triangles[i]],1);
        }
        glEnd();
        
        
        material.end();
        
        pointLight.disable();
        ofDisableLighting();
        
        debugShader.end();
        syphonIn->unbind();
    }
    
    /*    ofDisableDepthTest();
     depthFbo.draw(0,0);
     */
    ofNoFill();
    ofEnableAlphaBlending();
    if(wireframeAlpha > 0){
        ofSetColor(255*colorR,255*colorG,255*colorB, 255*wireframeAlpha);

        debugShader.begin();

        // debugShader.setUniformTexture("depthTex", depthFbo.getTextureReference(), 1);
        debugShader.setUniform1f("lightAmount", light);
        debugShader.setUniform1f("textureAmount", syphonOpacity);
        syphonIn->bind();
        material.setShininess(15);
        ofEnableLighting();

        pointLight.enable();
        material.begin();

        
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        //    glEnable(GL_LINE_SMOOTH);
        
        glBegin(GL_TRIANGLES);
        
        for(int i=0;i<mapping->triangles.size();i++){
            drawTriangleWireframe(subTriangles[mapping->triangles[i]]);
        }
        glEnd();
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        ofFill();
        
        syphonIn->unbind();
        material.end();
        pointLight.disable();
        debugShader.end();
    }
    ofDisableLighting();

    ofSetColor(255);
    //  glDisable(GL_LINE_SMOOTH);
    
}

void Triangles::update(){
    noiseSeed += noiseSeedSpeed * 1.0/MAX(10,ofGetFrameRate());
    if(noiseSeed > 1)
        noiseSeed = 0;
    for(int i=0;i<mapping->triangles.size();i++){
        SubTriangle * triangle = subTriangles[mapping->triangles[i]];
        float dist = triangle->getCenter().distance(ofPoint(OUTWIDTH*0.5,OUTHEIGHT));
        
        if(divideInvert){
            triangle->drawLevelGoal = MIN(divideTriangleSize,MAX(1,divideTriangleSize * ((3000-dist)/divideRadius)));
        } else {
            triangle->drawLevelGoal = MIN(divideTriangleSize,MAX(1,divideTriangleSize * dist/divideRadius));
        }
        
        triangle->noise = noise;
        triangle->noiseSeed = noiseSeed;

        triangle->update();
    }
    
    
    _lightPhase += 2*lightSpeed * 1.0 / MAX(10, MIN(ofGetFrameRate(),60));
    lightPos = ofVec3f(2596+1500*sin(_lightPhase),200,2000);
    
    center.x = 2996;
    center.y = 1200;
    
    pointLight.setPosition(lightPos);
    
   
}



void Triangles::parseOscMessage(ofxOscMessage *m){
    ContentScene::parseOscMessage(m);
    
    
 	vector<string> adrSplit = ofSplitString(m->getAddress(), "/");
	string rest = ofSplitString(m->getAddress(), "/"+adrSplit[1])[1];
    
	if(adrSplit[1] == "scene"+ofToString(index) || "/"+adrSplit[1] == oscAddress) {
        if( rest == "/syphonopacity/x" ) {
            syphonOpacity = m->getArgAsFloat(0);
	    }
/*        if( rest == "/dividecount/x" ) {
            divideCount = m->getArgAsFloat(0) * 6;
	    }*/
        if( rest == "/divideradius/x" ) {
            divideRadius = m->getArgAsFloat(0) * 2400;
	    }
        if( rest == "/divideinvert/x" ) {
            divideInvert = m->getArgAsFloat(0);
	    }
        if( rest == "/transitiontime/x" ) {
            transitionTime = m->getArgAsFloat(0)*5;
	    }
        if( rest == "/light/x" ) {
            light = m->getArgAsFloat(0);
	    }
        
        if( rest == "/lightspeed/x" ) {
            lightSpeed = m->getArgAsFloat(0);
	    }
        if( rest == "/directopacity/x" ) {
            directTextureOpacity = m->getArgAsFloat(0);
	    }
        if( rest == "/triangleColorR/x" ) {
            colorR = m->getArgAsFloat(0);
	    }
        if( rest == "/triangleColorG/x" ) {
            colorG = m->getArgAsFloat(0);
	    }
        if( rest == "/triangleColorB/x" ) {
            colorB = m->getArgAsFloat(0);
	    }
        
    }
    
}