#pragma once
#include "ofMain.h"


class particleShader {

	public:
		particleShader();
		~particleShader();
		
		//void setAttractPoints( vector <ofPoint> * attract );

		void reset(ofRectangle posInPrimary, ofRectangle posInSecondary);
		void resetEnd();
		void update();
		void draw();		
		
		ofPoint pos;
		ofPoint vel;
		ofPoint frc;
		
		float drag; 
		float uniqueVal;
		float scale;
		

		ofRectangle posIn1;
		ofRectangle posIn2;
		//particleMode mode;
		float radius;
		//vector <ofPoint> * attractPoints; 
};