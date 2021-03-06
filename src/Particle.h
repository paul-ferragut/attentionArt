//
//  particle.hpp
//  vectorField
//
//  Created by Caroline Record on 10/2/16.
//
//

#pragma once

#include "ofMain.h"

class Particle {
public:
    
    
    void setup(ofVec2f startPos);
    void move(ofVec2f dir);
    void draw();
    void stayOnScreen(ofRectangle boundsScreen);
	bool reset(ofRectangle boundsScreen);
    bool isOffScreen(ofRectangle boundsScreen);
    bool isDead();
    
    ofVec2f pos;
    int duration;
    
};