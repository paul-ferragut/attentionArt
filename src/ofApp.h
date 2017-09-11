#pragma once

#include "ofMain.h"
#include "ofxVectorField.h"
#include "Particle.h"
#include "ofxVoronoi.h"
#include "ofxGui.h"

#include "ofxFX.h"
#include "ofxGrayscale.h"//    grayscale;
#include "ofxInverse.h"    //  inverse;
#include "ofxGaussianBlur.h" //blur;
#include "ofxNormals.h"      //normals;
#include "ofxMask.h"        //mask;


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		vector <ofPoint> generateRandomPoints(int count, int seed, ofRectangle bounds);
		ofxVoronoi voronoi;
		vector<ofPoint> points;
		ofRectangle bounds;
		bool isBorder(ofPoint _pt);

		ofxVectorField vectorField;

		vector <Particle> particles;
		
		ofxPanel gui;
		ofxToggle colorMapDebug;
		ofxToggle vectorFieldDebug;
		ofxToggle centerVoroDebug;
		ofxFloatSlider scaleVectorfield;
		ofxFloatSlider animateVectorfield;


		ofxGrayscale    grayscale;
		ofxInverse      inverse;
		ofxGaussianBlur blur;
		ofxNormals      normals;
		ofxMask         mask;

		ofFbo           pingpong[2];

		ofShader        shaderPaint;

		int     timer, width, height;//, blurRadius;
		ofxIntSlider blurRadius;
		ofxToggle refreshPixel;
		ofxToggle imagePaint;
		ofxToggle paintDebug;

		ofFbo FBO;

		//bool    bDebug;
		//bool    bImage;



};
