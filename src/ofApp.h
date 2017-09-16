#pragma once

#include "ofMain.h"
#include "ofxVectorField.h"
#include "Particle.h"
#include "ofxVoronoi.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"

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
		void drawDebug();
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
	//	vector<ofPoint> points;
		ofRectangle bounds;
		bool isBorder(ofPoint _pt);

		ofxVectorField vectorField;

		vector <Particle> particles;
		
		ofxPanel gui1;
		ofxFloatSlider opacityRefresh;
		ofxFloatSlider opacityPaint;
		ofxToggle colorMapDebug;
		ofxToggle vectorFieldDebug;
		ofxToggle centerVoroDebug;
		ofxToggle relaxVoronoi;
		ofxFloatSlider scaleVectorfield;
		ofxFloatSlider animateVectorfield;

		ofxPanel gui2;
		ofxToggle gradientDebug;
		ofxColorSlider color[5];
		ofxFloatSlider positionColor[3];


		ofxGrayscale    grayscale;
		ofxInverse      inverse;
		ofxGaussianBlur blur;
		ofxNormals      normals;
		ofxMask         mask;

		ofFbo           pingpong[2];

		ofxAutoReloadedShader        shaderPaint;

		int     timer, width, height;//, blurRadius;
		ofxIntSlider blurRadius;
		ofxToggle refreshPixel;
		ofxToggle imagePaint;
		ofxToggle paintDebug;

		ofFbo FBO;


		ofShader shaderGradientMap;
		ofShader shaderGradient;

		ofFbo gradientFbo;
		int widthGrad;
		int heightGrad;

		//bool    bDebug;
		//bool    bImage;



};
