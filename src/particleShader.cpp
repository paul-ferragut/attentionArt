#include "particleShader.h"

//------------------------------------------------------------------
particleShader::particleShader(){
	//attractPoints = NULL;
}
particleShader::~particleShader() {
	//attractPoints = NULL;
}

//------------------------------------------------------------------
/*
void particleShader::setAttractPoints( vector <ofPoint> * attract ){
	attractPoints = attract;
}
*/
//------------------------------------------------------------------
void particleShader::reset(ofRectangle posInPrimary,ofRectangle posInSecondary){
	//the unique val allows us to set properties slightly differently for each particle
	uniqueVal = ofRandom(-10000, 10000);
	
	posIn1 = posInPrimary;
	posIn2 = posInSecondary;

	pos.x = ofRandom(posIn1.x, posIn1.x + posIn1.width);//ofRandomWidth();
	pos.y = ofRandom(posIn1.x, posIn1.x + posIn1.width);//ofRandomHeight();

	if (ofRandomf() > 0.6) {
		pos.x = ofRandom(posIn2.x, posIn2.x + posIn2.width);//ofRandomWidth();
		pos.y = ofRandom(posIn2.x, posIn2.x + posIn2.width);//ofRandomHeight();
	}
	
	vel.x = ofRandom(-3.9, 3.9);
	vel.y = ofRandom(-3.9, 3.9);
	
	frc   = ofPoint(0,0,0);
	
	radius = ofRandom(4.0, ofRandom(400, 800));

	//scale = ofRandom(0.5, 1.0);
//	mode = PARTICLE_MODE_NOISE;
//	if( mode == PARTICLE_MODE_NOISE ){
		drag  = ofRandom(0.97, 0.97);
	//	vel.y = fabs(vel.y) * 3.0; //make the particles all be going down
	//}else{
	//	drag  = ofRandom(0.95, 0.998);	
//	}
}

void particleShader::resetEnd()
{
	radius = ofRandom(4.0, ofRandom(400,800));

	pos.x = ofRandom(posIn1.x, posIn1.x + posIn1.width);//ofRandomWidth();
	pos.y = -(radius * 2);// ofRandom(posIn1.x, posIn1.x + posIn1.width);//ofRandomHeight();

	if (ofRandomf() > 0.6) {
		pos.x = ofRandom(posIn2.x, posIn2.x + posIn2.width);//ofRandomWidth();
		pos.y = -(radius*2);//ofRandom(posIn2.x, posIn2.x + posIn2.width);//ofRandomHeight();
	}

	vel.x = ofRandom(-3.9, 3.9);
	vel.y = ofRandom(-3.9, 3.9);

	frc = ofPoint(0, 0, 0);

	

	//scale = ofRandom(0.5, 1.0);
	//	mode = PARTICLE_MODE_NOISE;
	//	if( mode == PARTICLE_MODE_NOISE ){
	drag = ofRandom(0.97, 0.97);
	//	vel.y = fabs(vel.y) * 3.0; //make the particles all be going down
	//}else{
	//	drag  = ofRandom(0.95, 0.998);	
	//	}
}

//------------------------------------------------------------------
void particleShader::update(){

	//1 - APPLY THE FORCES BASED ON WHICH MODE WE ARE IN 
	/*
	if( mode == PARTICLE_MODE_ATTRACT ){
		ofPoint attractPt(ofGetMouseX(), ofGetMouseY());
		frc = attractPt-pos; // we get the attraction force/vector by looking at the mouse pos relative to our pos
		frc.normalize(); //by normalizing we disregard how close the particle is to the attraction point 
		
		vel *= drag; //apply drag
		vel += frc * 0.6; //apply force
	}
	else if( mode == PARTICLE_MODE_REPEL ){
		ofPoint attractPt(ofGetMouseX(), ofGetMouseY());
		frc = attractPt-pos; 
		
		//let get the distance and only repel points close to the mouse
		float dist = frc.length();
		frc.normalize(); 
		
		vel *= drag; 
		if( dist < 150 ){
			vel += -frc * 0.6; //notice the frc is negative 
		}else{
			//if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy. 			
			frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);
			frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);
			vel += frc * 0.04;
		}
	}
	else if( mode == PARTICLE_MODE_NOISE ){
		*/
		//lets simulate falling snow 
		//the fake wind is meant to add a shift to the particles based on where in x they are
		//we add pos.y as an arg so to prevent obvious vertical banding around x values - try removing the pos.y * 0.006 to see the banding
		float fakeWindX = ofSignedNoise(pos.x * 0.003, pos.y * 0.006, ofGetElapsedTimef() * 0.6);
		
		frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 0.6;
		frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, ofGetElapsedTimef()*0.2) * 0.09 + 0.18;
	
		vel *= drag; 
		vel += frc * 0.4;
		
		//we do this so as to skip the bounds check for the bottom and make the particles go back to the top of the screen
	//	if( pos.y + vel.y > ofGetHeight() ){
	//		pos.y -= ofGetHeight();
	//	}
		/*
	}
	else if( mode == PARTICLE_MODE_NEAREST_POINTS ){
		
		if( attractPoints ){

			//1 - find closest attractPoint 
			ofPoint closestPt;
			int closest = -1; 
			float closestDist = 9999999;
			
			for(unsigned int i = 0; i < attractPoints->size(); i++){
				float lenSq = ( attractPoints->at(i)-pos ).lengthSquared();
				if( lenSq < closestDist ){
					closestDist = lenSq;
					closest = i;
				}
			}
			
			//2 - if we have a closest point - lets calcuate the force towards it
			if( closest != -1 ){
				closestPt = attractPoints->at(closest);				
				float dist = sqrt(closestDist);
				
				//in this case we don't normalize as we want to have the force proportional to distance 
				frc = closestPt - pos;
		
				vel *= drag;
				 
				//lets also limit our attraction to a certain distance and don't apply if 'f' key is pressed
				if( dist < 300 && dist > 40 && !ofGetKeyPressed('f') ){
					vel += frc * 0.003;
				}else{
					//if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy. 			
					frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);
					frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);
					vel += frc * 0.4;
				}
				
			}
		
		}
		
	}
	*/
	
	//2 - UPDATE OUR POSITION
	
	pos += vel; 
	
	
	//3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN 
	//we could also pass in bounds to check - or alternatively do this at the ofApp level
	if( pos.x > ofGetWidth() ){
		pos.x = ofGetWidth();
		vel.x *= -1.0;
	}else if( pos.x < posIn2.x){
		pos.x = posIn2.x;
		vel.x *= -1.0;
	}
	if( pos.y > ofGetHeight()+(radius*2) ){
		//pos.y = ofGetHeight();
		//vel.y *= -1.0;
		resetEnd();
	}
	else if( pos.y < -(radius*2) ){
		pos.y = -(radius*2);
		vel.y *= -1.0;
	}	
		
}

//------------------------------------------------------------------
void particleShader::draw(){

	/*
	if( mode == PARTICLE_MODE_ATTRACT ){
		ofSetColor(255, 63, 180);
	}
	else if( mode == PARTICLE_MODE_REPEL ){
		ofSetColor(208, 255, 63);
	}
	else if( mode == PARTICLE_MODE_NOISE ){
		ofSetColor(99, 63, 255);
	}
	else if( mode == PARTICLE_MODE_NEAREST_POINTS ){
		ofSetColor(103, 160, 237);
	}
	*/
	ofSetColor(255, 255, 255);
	ofNoFill();
	ofDrawCircle(pos.x, pos.y, radius);
	ofFill();
}

