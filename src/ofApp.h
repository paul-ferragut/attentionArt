#pragma once

#include "ofMain.h"
#include "ofxVectorField.h"
#include "Particle.h"
#include "ofxVoronoi.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"
#include "ofxMtlMapping2D.h"
#include "ofxNetwork.h"

#include "ofxFX.h"
#include "ofxGrayscale.h"//    grayscale;
#include "ofxInverse.h"    //  inverse; //TO DO REMOVE
#include "ofxGaussianBlur.h" //blur;
#include "ofxNormals.h"      //normals;
#include "ofxMask.h"        //mask;

#include "particleShader.h"

#include "ofxMtlMapping2D.h"

#define NUM_PALETTES 5

#define STRESS 0
#define	INBETWEEN 1
#define RELAX 2
#define IDLE 3

#define DURATION 20000 //1000 millisecond 


#define UDPLOWRANGE 1000 //1000 millisecond 
#define UDPHIGHRANGE 2000

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
		void drawWithPost();

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
		ofxToggle debugDistort;

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
		float prevMainVarGSRAveraged;
		ofxToggle beat;
		ofxIntSlider averaging;
		ofxIntSlider stressThreshold;
		ofxIntSlider relaxThreshold;
		ofxIntSlider stressStateTrigger;
		ofxIntSlider relaxStateTrigger;
		ofxIntSlider idleStateTrigger;
		ofxToggle usePreset;
		ofxToggle restartPreset;
		ofxIntSlider weightPreset;
		ofxToggle useUDPRead;

		int stressCounter;
		int relaxCounter;
		int stressPaletteNum;
		int relaxPaletteNum;

		int idleCounter;
	
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

		ofxToggle useMapping;

		ofFbo FBO;
		ofFbo FBO2;

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

		float edgeStressVal[3];
		float edgeRelaxVal[3];

		ofShader shaderPost;
		ofxFloatSlider val[3];
		static const int arrayLength = 10;
		ofVec2f arrayUniform[arrayLength];
		float arrayParticleSize[arrayLength];
		vector<particleShader> p;

		bool fullScreen;
		bool guiVisible;
		bool screenGrab;

		//stored for IDLE Transition
		ofColor colLastIdle[5];
		float posLastIdle[3];
		float valLastIdle[3];
		float animateLastIdle;
		float scaleVecLastIdle;
		float idleTransitionCounter;

		ofPolyline line;
		void loadPreset();
		float getPresetVal(float percent);

		float timerGSR;
		float lastTimeMeasured;

		void restartPresetTimeLine();

		void setupUdp();
		float updateUdp();
		ofxUDPManager udpConnection;
		float UDPread;
		vector<float>udpHistory;

		private:
			ofxMtlMapping2D* _mapping;
};
