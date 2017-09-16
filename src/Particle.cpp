//
//  particle.cpp
//  vectorField
//
//  Created by Caroline Record on 10/2/16.
//
//

#include "Particle.h"

void Particle::setup(ofVec2f startPos){
    pos = startPos;
    duration = 0;
}

void Particle::move(ofVec2f dir){
    pos += dir;
}

void Particle::draw(){
    //ofSetColor(255, 255, 255);
    ofDrawCircle(pos.x, pos.y, 5);
    duration ++;
}

void Particle::stayOnScreen(ofRectangle boundsScreen){
    if( pos.x < boundsScreen.x) pos.x += boundsScreen.getWidth();
    if( pos.x >= boundsScreen.getWidth()) pos.x -= boundsScreen.getWidth();
    if( pos.y <  boundsScreen.y) pos.y += boundsScreen.getHeight();
    if( pos.y >= boundsScreen.getHeight()) pos.y -= boundsScreen.getHeight();
}


bool Particle::reset(ofRectangle boundsScreen) {
	ofRectangle bounds(boundsScreen.x + 1, boundsScreen.y + 1, boundsScreen.width - 1, boundsScreen.height - 1);
	boundsScreen = bounds;

	if (pos.x < boundsScreen.x) 
	{ 
		pos.set(ofRandom(boundsScreen.x, boundsScreen.width), ofRandom(boundsScreen.y, boundsScreen.height)); 
		return true;
	}
	if (pos.x >= boundsScreen.getWidth()) 
	{
		pos.set(ofRandom(boundsScreen.x, boundsScreen.width), ofRandom(boundsScreen.y, boundsScreen.height));
		return true;
	}
	if (pos.y < boundsScreen.y)
	{
		pos.set(ofRandom(boundsScreen.x, boundsScreen.width), ofRandom(boundsScreen.y, boundsScreen.height));
		return true;
	}
	if (pos.y >= boundsScreen.getHeight()) 
	{ 
		pos.set(ofRandom(boundsScreen.x, boundsScreen.width), ofRandom(boundsScreen.y, boundsScreen.height)); 
		return true;
	}
	return false;
}

bool Particle::isOffScreen(ofRectangle boundsScreen){
    if( pos.x <boundsScreen.x || pos.x >= boundsScreen.getWidth() || pos.y < boundsScreen.y || pos.y >= boundsScreen.getHeight()){
        
        return true;
    }
    
    return false;
}

bool Particle::isDead(){
    return duration >= 10000;
}
