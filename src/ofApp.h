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

#define NUM_PALETTES 5

#define STRESS 0
#define	INBETWEEN 1
#define RELAX 2
#define IDLE 3

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

		void setupPalettes();

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
		ofxIntSlider maxVoroPtNum;
		ofxToggle relaxVoronoi;
		ofxFloatSlider scaleVectorfield;
		ofxFloatSlider animateVectorfield;

		ofxPanel gui2;
		ofxToggle gradientDebug;
		ofxColorSlider color[5];
		ofxFloatSlider positionColor[3];

		ofColor colorRelaxCurrent[5];
		float positionColorRelaxCurrent[3];
		ofColor colorStressCurrent[5];
		float positionColorStressCurrent[3];

		ofxPanel gui3;
		ofxToggle useMainVarGSR;
		ofxIntSlider mainVarGSR;
		ofxIntSlider mainVarGSRAveraged;
		ofxToggle beat;
		ofxIntSlider averaging;
		ofxIntSlider stressThreshold;
		ofxIntSlider relaxThreshold;
		ofxIntSlider stressStateTrigger;
		ofxIntSlider relaxStateTrigger;

		int stressCounter;
		int relaxCounter;
		int stressPaletteNum;
		int relaxPaletteNum;
	
		vector<ofPolyline> trail;
		vector<float> yGraph;
		float xGraph, prevXGraph;
		float radius, initTime, t;
		vector<float> mainVarHistory;


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

		int interactionState;
		int prevInteractionState;
		string stateString[4];


		vector<vector<ofColor>>colorRelax;
		vector<vector<ofColor>>colorStress;

		vector<vector<float>>colorPosRelax;
		vector<vector<float>>colorPosStress;

		float edgeStressScale;
		float edgeStressAnimate;
		float edgeRelaxScale;
		float edgeRelaxAnimate;

};
