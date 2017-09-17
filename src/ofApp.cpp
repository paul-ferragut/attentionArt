#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofEnableAlphaBlending();
	ofBackground(255);


	//GUI1
	gui1.setup("Design", "settings1.xml",ofGetWidth()-210,0);
	gui1.add(colorMapDebug.setup("colorMapDebug",true));
	gui1.add(vectorFieldDebug.setup("vectorFieldDebug", true));
	gui1.add(centerVoroDebug.setup("centerVoroDebug", true));
	gui1.add(relaxVoronoi.setup("relax Voronoi", true));
	gui1.add(scaleVectorfield.setup("scale vectorfield", 1, 0.1, 7));
	gui1.add(animateVectorfield.setup("animate vectorfield", 0.5, 0.0,5.0));
	gui1.add(maxVoroPtNum.setup("Maximum Voronoi", 300, 100, 1000));
	gui1.add(blurRadius.setup("blurRadius", 10, 0.0, 20.0));
	gui1.add(refreshPixel.setup("refreshPixel", false));
	gui1.add(imagePaint.setup("imagePaint", false));
	gui1.add(paintDebug.setup("paintDebug", false));
	gui1.add(opacityPaint.setup("opacity paint", 255, 0.0, 255));
	gui1.add(opacityRefresh.setup("opacity refresh", 255, 0.0, 255));
	gui1.add(debugDistort.setup("debug distort", false));
	for (int i = 0;i < 3;i++) {
		gui1.add(val[i].setup("val" + ofToString(i), 0.0, 0.0, 1.0));
	}


	gui1.loadFromFile("settings1.xml");

	edgeStressScale=5;
	edgeStressAnimate=4.5;
	edgeRelaxScale=0.4;
	edgeRelaxAnimate=0.5;


	edgeStressVal[0]=0.5;
	edgeStressVal[1] =1.0;
	edgeStressVal[2] =0.1;
	edgeRelaxVal[0]=0;
	edgeRelaxVal[1] = 0;
	edgeRelaxVal[2] = 0;

	//GUI2
	gui2.setup("Color","settings2.xml", ofGetWidth() - 420,0);
	gui2.add(gradientDebug.setup("gradient", false));
	for (int i = 0;i < 5;i++) {
		gui2.add(color[i].setup("color " + ofToString(i), 0, 0, 255));
	}
	for (int i = 0;i < 3;i++) {
		gui2.add(positionColor[i].setup("pos color" + ofToString(i), (1.0 / 5.0)*(i + 1), 0, 1.0));
	}
	gui2.loadFromFile("settings2.xml");
	
	
	setupPalettes();
	stressPaletteNum = ofRandom(NUM_PALETTES);
	relaxPaletteNum = ofRandom(NUM_PALETTES);

	for (int i = 0;i < 5;i++) {
		colorRelaxCurrent[i] = colorRelax[relaxPaletteNum][i];
	}
	for (int i = 0;i < 3;i++) {
		positionColorRelaxCurrent[i] = colorPosRelax[relaxPaletteNum][i];
	}
	for (int i = 0;i < 5;i++) {
		colorStressCurrent[i] = colorStress[stressPaletteNum][i];
	}
	for (int i = 0;i < 3;i++) {
		positionColorStressCurrent[i] = colorPosStress[stressPaletteNum][i];
	}


	//GUI2
	gui3.setup("Interactivity", "settings3.xml", ofGetWidth() - 630, 0);
	gui3.add(useMainVarGSR.setup("use Main Var GSR", false));
	gui3.add(mainVarGSR.setup("main Var GSR", 0.0, 0.0, 100.0));
	gui3.add(mainVarGSRAveraged.setup("main Var GSR averaged", 0.0, 0.0, 100.0));
	gui3.add(beat.setup("beat", false));
	gui3.add(averaging.setup("averaging main var", 10, 2, 300));
	
	gui3.add(relaxThreshold.setup("relax Threshold", 20, 1, 100));
	gui3.add(stressThreshold.setup("stress Threshold", 80, 0, 100));

	gui3.add(relaxStateTrigger.setup("relax state trigger", 10, 1, 600));
	gui3.add(stressStateTrigger.setup("stress state trigger", 10, 1, 600));

	gui3.loadFromFile("settings3.xml");

	relaxCounter = 0;
	stressCounter = 0;

	trail.resize(2);
	yGraph.resize(2);
	xGraph = 0;
	prevXGraph = 0;
	//int stressPaletteNum;
	//int relaxPaetteNum;

	bounds = ofRectangle(0, 0, ofGetWidth() / 2, ofGetHeight() / 2);
	vectorField.setup(bounds.getWidth(), bounds.getHeight(), 5);
	vectorField.randomize();

	int seed = 33;
	vector<ofPoint>points = generateRandomPoints(maxVoroPtNum, seed, bounds);

	for (int i = 0;i < points.size();i++) {
		Particle newParticle;
		ofVec2f pos = points[i];//(ofRandom(0, bounds.getWidth()), ofRandom(0, bounds.getHeight()));
		newParticle.setup(pos);
		particles.push_back(newParticle);
	}

	voronoi.setBounds(bounds);
	voronoi.setPoints(points);
	voronoi.generate();
	points.clear();
	for (auto cell : voronoi.getCells()) {
		points.push_back(cell.pt);
	}

	widthGrad = 510;
	heightGrad = 1;
	gradientFbo.allocate(widthGrad, heightGrad, GL_RGB);
	shaderGradient.load("gradient");
	shaderGradientMap.load("gradientMap");

	width = ofGetWidth();
	height = ofGetHeight();

	grayscale.allocate(width, height);
	inverse.allocate(width, height);
	blur.allocate(width, height);
	blur.setRadius(1);
	blur.setPasses(1);
	normals.allocate(width, height);
	mask.allocate(width, height);
	timer = 0;
	
	for (int i = 0; i < 2; i++) {
		pingpong[i].allocate(width, height, GL_RGBA);
		pingpong[i].begin();
		ofClear(0);
		pingpong[i].end();
	}

	FBO.allocate(width, height, GL_RGB, 8);
	FBO2.allocate(width, height, GL_RGB, 8);

	shaderPaint.load("shaderPaint");

	stateString[0] = "STRESS";
	stateString[1] = "INBETWEEN";
	stateString[2] = "RELAX";
	stateString[3] = "IDLE";


	shaderPost.load("POST");
	int num = arrayLength;
	p.assign(num, particleShader());
	//currentMode = PARTICLE_MODE_ATTRACT;

	//currentModeStr = "1 - PARTICLE_MODE_ATTRACT: attracts to mouse";

	ofRectangle distortRect1(0,0,ofGetWidth(),ofGetHeight());
		ofRectangle distortRect2(0, 0, ofGetWidth(), ofGetHeight());
	for (unsigned int i = 0; i < p.size(); i++) {
		//	p[i].setMode(currentMode);
		//	p[i].setAttractPoints(&attractPointsWithMovement);;
		p[i].reset(distortRect1, distortRect2);
	}

	fullScreen=false;
	guiVisible=true;
	screenGrab=false;
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle("FPS:" + ofToString(ofGetFrameRate()));

	if (useMainVarGSR) {
		 
		bool inB = true;
		//STATE MANAGEMENT
		if (mainVarGSRAveraged <= stressThreshold) {
			stressCounter++;
		}
		else {
			stressCounter--;
		}

		if (stressCounter < 0) {
			stressCounter = 0;
		}
		if (stressCounter > stressStateTrigger*1.4) {
			stressCounter = stressStateTrigger*1.4;
		}
		if (stressCounter > stressStateTrigger) {
			interactionState = STRESS;
			inB = false;
		}
		//
		if (mainVarGSRAveraged >= relaxThreshold) {
			relaxCounter++;
		}
		else {
			relaxCounter--;
		}

		if (relaxCounter < 0) {
			relaxCounter = 0;
		}
		if (relaxCounter > relaxStateTrigger*1.4) {
			relaxCounter = relaxStateTrigger*1.4;
		}
		if (relaxCounter > relaxStateTrigger) {
			interactionState = RELAX;
			inB = false;
		}

		if (inB)interactionState = INBETWEEN;


		//NEW STATE EVENT
		if (prevInteractionState != interactionState) {

			if (interactionState == RELAX) {
				imagePaint = true;
				refreshPixel = false;
				relaxVoronoi = false;

				stressPaletteNum = ofRandom(NUM_PALETTES);
				for (int i = 0;i < 5;i++) {
					colorStressCurrent[i] = colorStress[stressPaletteNum][i];
				}
				for (int i = 0;i < 3;i++) {
					positionColorStressCurrent[i] = colorPosStress[stressPaletteNum][i];
				}
			}

			if (interactionState == IDLE) {
				relaxVoronoi = true;
			}

			if (interactionState == STRESS) {
					relaxPaletteNum = ofRandom(NUM_PALETTES);
					for (int i = 0;i < 5;i++) {
						colorRelaxCurrent[i] = colorRelax[relaxPaletteNum][i];
					}
					for (int i = 0;i < 3;i++) {
						positionColorRelaxCurrent[i] = colorPosRelax[relaxPaletteNum][i];
					}
			}
			if (prevInteractionState == RELAX) { //&& interactionState==INBETWEEN STRESS IDLE
				refreshPixel = true;
			}
		}
		prevInteractionState = interactionState;

		//CONSTANT UPDATING
		//COLORS UPDATING
		float normVal=ofMap(mainVarGSRAveraged,relaxThreshold ,stressThreshold , 0.0, 1.00,true);
		for (int i = 0;i < 5;i++) {
			float redN = ofLerp(colorRelaxCurrent[i].r, colorStressCurrent[i].r, normVal);
			float greenN = ofLerp(colorRelaxCurrent[i].g, colorStressCurrent[i].g, normVal);
			float blueN = ofLerp(colorRelaxCurrent[i].b, colorStressCurrent[i].b, normVal);
			color[i] = ofColor(redN, greenN, blueN);
		}
		for (int i = 0;i < 3;i++) {
			positionColor[i]= ofLerp(positionColorRelaxCurrent[i], positionColorStressCurrent[i], normVal);
		}
		//SPEED UPDATING

		scaleVectorfield= ofLerp(edgeRelaxScale,edgeStressScale , normVal);
		animateVectorfield= ofLerp(edgeRelaxAnimate, edgeStressAnimate, normVal);

		for (int i = 0;i < 3;i++) {
			val[i] = ofLerp(edgeRelaxVal[i], edgeStressVal[i], normVal);
		}

	}	

	gradientFbo.begin();
	shaderGradient.begin();
	shaderGradient.setUniform2f("u_resolution", widthGrad, heightGrad);
	for (int i = 0;i < 5;i++) {
		ofFloatColor col = ofColor(color[i]);
		//cout << "col" << col << endl;
		shaderGradient.setUniform3f("col" + ofToString(i + 1), col.r, col.g, col.b);
	}
	shaderGradient.setUniform1f("p1", positionColor[0]);
	shaderGradient.setUniform1f("p2", positionColor[1]);
	shaderGradient.setUniform1f("p3", positionColor[2]);
	ofDrawRectangle(0, 0, widthGrad, heightGrad);
	shaderGradient.end();
	gradientFbo.end();

	//vectorField.randomize();
	vectorField.animate(animateVectorfield*0.004);
	vectorField.normalize();
	vectorField.scale(scaleVectorfield);
	vectorField.bias(0, 0);
	vectorField.blur();


	if (relaxVoronoi) {
	
		vector<ofxVoronoiCell>cells = voronoi.getCells();
		for (int i = 0; i <cells.size(); i++) {
			particles[i].pos = cells[i].pt;
		}

	}
	else {
		for (int i = 0; i<particles.size(); i++) {
			particles[i].move(vectorField.getVectorInterpolated(particles[i].pos.x, particles[i].pos.y, bounds.getWidth(), bounds.getHeight()));
			bool reachedBounds=particles[i].reset(bounds);
			if (reachedBounds) {
			//	particles.erase(particles.begin() + i);
			}
		}

		if (particles.size() < maxVoroPtNum && ofGetFrameNum()%4==0) {
			int seed = 33;
			//vector<ofPoint>points = generateRandomPoints(1, seed, bounds);
			vector<ofPoint>nPoints = generateRandomPoints(1, seed, bounds);
			for (int i = 0;i < nPoints.size();i++) {
				Particle newParticle;
				ofVec2f pos = nPoints[i];//(ofRandom(0, bounds.getWidth()), ofRandom(0, bounds.getHeight()));
				newParticle.setup(pos);
				particles.push_back(newParticle);
				//cout << "new pt:" << ofGetFrameNum() << endl;
			}
		}
	}



	timer++;

	mask.setTexture(grayscale.getTexture(), 1);
	mask.update();
	blur.setRadius(blurRadius);

	ofSetColor(255, 255);

	if (imagePaint) {
		grayscale.setTexture(FBO.getTextureReference());
		grayscale.update();
		inverse << grayscale;
		inverse.update();
		blur << inverse;
		blur.update();
		normals.update();

		pingpong[0].begin();
		/*
		shaderGradientMap.begin();
		shaderGradientMap.setUniformTexture("gradient", gradientFbo.getTexture(), 1);
		shaderGradientMap.setUniform1f("gradientWidth", gradientFbo.getWidth());
		FBO.draw(0, 0);
		shaderGradientMap.end();
		*/
		//drawWithPost();
		ofSetColor(255, 255);
		FBO.draw(0, 0);
		pingpong[0].end();

		pingpong[1].begin();	
		/*
		shaderGradientMap.begin();
		shaderGradientMap.setUniformTexture("gradient", gradientFbo.getTexture(), 1);
		shaderGradientMap.setUniform1f("gradientWidth", gradientFbo.getWidth());
		
		FBO.draw(0, 0);	
		shaderGradientMap.end();
		*/
		//drawWithPost();
		ofSetColor(255, 255);
		FBO.draw(0, 0);

		pingpong[1].end();

		imagePaint = false;
	}
	else {
		grayscale.setTexture(FBO.getTextureReference());
		grayscale.update();
		inverse << grayscale;
		inverse.update();
		blur << inverse;
		blur.update();
		normals << blur;
		normals.update();

		pingpong[timer % 2].begin();//either 0 or 1
							
		if (refreshPixel) {		
			ofSetColor(255, 255, 255, opacityRefresh);
			FBO.draw(0, 0);
			//refreshPixel = false;
		}

		shaderPaint.begin();
		shaderPaint.setUniformTexture("normals", normals.getTexture(), 1);
		shaderPaint.setUniformTexture("dampMap", mask.getTexture(), 2);



		pingpong[(timer + 1) % 2].draw(0, 0);//either 0 or 1, opposite of [timer % 2]
	

		shaderPaint.end();
		pingpong[timer % 2].end();
	}

	 mask.begin();
	 ofClear(0,0);
	 mask.end();




	 t = ofGetElapsedTimef();
	 if (trail[0].size() == 0) {
		 initTime = t;
	 }
	 t = t - initTime;
	 int speed = 100;
	 xGraph = int(t*speed) % ((ofGetWidth()/2));
	 yGraph[0] = mainVarGSRAveraged;
	 yGraph[1] = mainVarGSR;

	 for (int i = 0;i<(int)trail.size();i++) {
		 if (xGraph<prevXGraph) {
			 trail[i].clear();
		 }
		 else {
			 trail[i].addVertex(ofPoint(xGraph, yGraph[i]));

		 }
	 }

	 prevXGraph = xGraph;

	 if (mainVarHistory.size() >= averaging) {
		 mainVarHistory.push_back(mainVarGSR);
		 mainVarHistory.erase(mainVarHistory.begin());
	 }
	 else {
		 mainVarHistory.push_back(mainVarGSR);
	 }
	 float averageSum = 0;
	 for (int i = 0;i < mainVarHistory.size();i++) {
		 averageSum += mainVarHistory[i];
	 }
	 mainVarGSRAveraged = averageSum / mainVarHistory.size();


	 for (unsigned int i = 0; i < p.size(); i++) {
		 //p[i].setMode(currentMode);
		 p[i].update();
		 arrayParticleSize[i] = ofMap(p[i].radius, 0, (ofGetWidth() + ofGetHeight()) / 2, 0, 1.0);
	 }

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255, 255, 255);

	ofSetLineWidth(0);
	ofNoFill();
	ofSetColor(220);
	ofDrawRectangle(bounds);

	if (relaxVoronoi) {	
		voronoi.relax();
	}
	else {
		vector<ofPoint>pts;
		for (int i = 0; i < particles.size(); i++) {
			pts.push_back(particles[i].pos);
		}
		voronoi.setPoints(pts);
		voronoi.generate();
	}

	FBO.begin();
	vector <ofxVoronoiCell> cells = voronoi.getCells();
	for (int i = 0; i<cells.size(); i++) {
		
		ofSetColor(0);
		ofColor gray= vectorField.noiseValAtPos(cells[i].pt);

		//ofNoFill();
		ofMesh mesh;
		//
		mesh.setMode(OF_PRIMITIVE_TRIANGLES);
			
		int indexCount=0;
		for (int j = 0; j < cells[i].pts.size(); j++) {

			ofVec2f pt1, pt2, pt3;

			pt1 = cells[i].pts[j];

			if (j == cells[i].pts.size() - 1) {
				pt2 = cells[i].pts[0];
			}
			else {
				pt2 = cells[i].pts[j + 1];
			}

			pt3 = cells[i].pt;

			ofVec2f centroid = (pt1+pt2+pt3 / 3);
			ofColor centerCol = vectorField.noiseValAtPos(centroid);

			mesh.addVertex(pt1);mesh.addColor(centerCol);
			mesh.addVertex(pt2);mesh.addColor(centerCol);
			mesh.addVertex(pt3);mesh.addColor(centerCol);

		}

		ofPushMatrix();		
		ofScale(2, 2);

		ofSetColor(255, 255, 255, 255);

		mesh.draw();
		
		ofPopMatrix();

	}
	FBO.end();

	ofEnableAlphaBlending();
	
	drawWithPost();

	ofSetColor(255, 255, 255, opacityPaint);
	pingpong[timer % 2].draw(0, 0);//,ofGetWidth(),ofGetHeight());

	if(guiVisible)
	drawDebug();

}

void ofApp::drawDebug() {
	//DEBUG
	ofFill();
	if (colorMapDebug) {
		for (int i = 0; i<bounds.width; i+=4) {
			for (int j = 0; j<bounds.height; j += 4) {
				ofSetColor(vectorField.noiseValAtPos(ofVec2f(i, j)));
				ofDrawRectangle(bounds.width+i, j, 4, 4);
			}
		}
	}

	if (paintDebug) {
		ofSetColor(255);
		grayscale.draw(0, 0, width*0.25, height*0.25);
		inverse.draw(width*0.25, 0, width*0.25, height*0.25);
		normals.draw(width*0.5, 0, width*0.25, height*0.25);
	}

	
	ofFill();
	if(centerVoroDebug){
		ofPushMatrix();
		ofScale(2, 2);
		ofSetColor(20, 20, 20);
		for (int i = 0; i<particles.size(); i++) {
			particles[i].draw();
		}

		ofPopMatrix();

	}

	if(vectorFieldDebug){
		ofSetColor(20, 20, 20);
		vectorField.draw();
	}

	ofSetColor(255, 255, 255);
	if(gradientDebug)
	gradientFbo.draw(5, ofGetHeight()-105, ofGetWidth()/2 - 10, 100);


	ofFill();
	for (int i = 0;i<(int)trail.size();i++) {
		ofEnableSmoothing();
		ofPushMatrix();
		ofTranslate(ofGetWidth() / 2 -5, ofGetHeight() - ((105 * i) + 105));
		ofDrawBitmapString("State: " + stateString[interactionState], 15, 15);
		ofSetColor(255, 255, 255, 150);
		ofDrawRectangle(0, 0, ofGetWidth() / 2, 100);
		ofSetColor(50, 50, 255);
		ofLine(0, relaxThreshold, ofGetWidth() / 2, relaxThreshold);
		ofSetColor(255, 50, 50);
		ofLine(0, stressThreshold, ofGetWidth() / 2, stressThreshold);
		ofSetColor(0, 0, 0, 255);
		if (interactionState == STRESS) {
			ofSetColor(255, 50, 50);
		}
		if (interactionState == RELAX) {
			ofSetColor(50, 50, 255);
		}
		if (interactionState == INBETWEEN) {
			ofSetColor(0, 0, 0);
		}
		ofDrawCircle(xGraph - 5, yGraph[i], 6);
		trail[i].draw();
		ofPopMatrix();


		ofDisableSmoothing();
	}

	if (debugDistort) {
	for (unsigned int i = 0; i < p.size(); i++) {
		p[i].draw();
		ofDrawBitmapStringHighlight(ofToString(i), p[i].pos);
	}
	}

	

	gui1.draw();
	gui2.draw();
	gui3.draw();
}

void ofApp::exit()
{
	gui1.saveToFile("settings1.xml");
	gui2.saveToFile("settings2.xml");
	gui3.saveToFile("settings3.xml");
}

//--------------------------------------------------------------
vector <ofPoint> ofApp::generateRandomPoints(int count, int seed, ofRectangle bounds) {
	vector <ofPoint> points;
	ofSeedRandom(seed);

	for (int i = 0; i<count; i++) {
		ofPoint newPoint = ofPoint(
			ofRandom(bounds.x, bounds.width),
			ofRandom(bounds.y, bounds.height)
		);

		points.push_back(newPoint);
	}

	return points;
}

//--------------------------------------------------------------
bool ofApp::isBorder(ofPoint _pt) {
	ofRectangle bounds = voronoi.getBounds();

	return (_pt.x == bounds.x || _pt.x == bounds.x + bounds.width
		|| _pt.y == bounds.y || _pt.y == bounds.y + bounds.height);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == 'c') {
		gui2.saveToFile( ofGetTimestampString() + ".xml" );
	}

	if (key == 'f') {
		fullScreen = !fullScreen;
		ofToggleFullscreen();
	}
	if (key == 'g') {
		guiVisible = !guiVisible;
	}
	if (key == 's') {
		screenGrab = !screenGrab;
		ofSaveScreen(ofGetTimestampString() + ".png");
	}



}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::setupPalettes()
{	
	
	for (int i = 0;i < NUM_PALETTES;i++) {

		ofXml xml;
		xml.load("relax/"+ofToString(i+1)+".xml");

		vector<ofColor>indexCol;
		for (int j = 0;j < 5;j++) {
			vector<string>parsedCol = ofSplitString(xml.getValue("color_" + ofToString(j)), ",");
			ofColor newCol(ofToInt(parsedCol[0]), ofToInt(parsedCol[1]), ofToInt(parsedCol[2]), ofToInt(parsedCol[3]));
			indexCol.push_back(newCol);
		}
		colorRelax.push_back(indexCol);

		vector<float>indexPosCol;
		for (int j = 0;j < 3;j++) {
			string value = xml.getValue("pos_color" + ofToString(j));
			indexPosCol.push_back(ofToFloat(value));
		}
		colorPosRelax.push_back(indexPosCol);

	}

	for (int i = 0;i < NUM_PALETTES;i++) {

		ofXml xml;
		xml.load("stress/" + ofToString(i + 1) + ".xml");

		vector<ofColor>indexCol;
		for (int j = 0;j < 5;j++) {
			vector<string>parsedCol = ofSplitString(xml.getValue("color_" + ofToString(j)), ",");
			ofColor newCol(ofToInt(parsedCol[0]), ofToInt(parsedCol[1]), ofToInt(parsedCol[2]), ofToInt(parsedCol[3]));
			indexCol.push_back(newCol);
		}
		colorStress.push_back(indexCol);

		vector<float>indexPosCol;
		for (int j = 0;j < 3;j++) {
			string value = xml.getValue("pos_color" + ofToString(j));
			indexPosCol.push_back(ofToFloat(value));
		}
		colorPosStress.push_back(indexPosCol);

	}

	//for (int i = 0;i < 5;i++) {
	//	for (int j = 0;j < 3;j++) {
			//cout << i << "i:" << colorPosRelax[i][j] << endl;
	//	}
	//}
	
}


void ofApp::drawWithPost() {


	FBO2.begin();
	ofClear(255);
	shaderGradientMap.begin();
	shaderGradientMap.setUniformTexture("gradient", gradientFbo.getTexture(), 1);
	shaderGradientMap.setUniform1f("gradientWidth", gradientFbo.getWidth());
	ofSetColor(255, 255, 255, 255);
	FBO.draw(0, 0);
	shaderGradientMap.end();
	FBO2.end();

	ofEnableAlphaBlending();
	FBO.begin();
	ofClear(255);
	shaderPost.begin();
	shaderPost.setUniform1f("elapsedTime", ofGetElapsedTimeMillis()*0.00003);
	shaderPost.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
	shaderPost.setUniform1f("alpha", 1.0);
	for (int i = 0;i < p.size();i++) {
		arrayUniform[i].x = p[i].pos.x;
		arrayUniform[i].y = ofMap(p[i].pos.y, 0, ofGetHeight(), ofGetHeight(), 0);
	}
	shaderPost.setUniform2fv("posP", &arrayUniform[0].x, arrayLength);
	shaderPost.setUniform1fv("radiusP", &arrayParticleSize[0], arrayLength);
	for (int i = 0;i < 3;i++) {
		shaderPost.setUniform1f("val" + ofToString(i + 1), (float)val[i]);
	}
	shaderPost.setUniformTexture("fboTexture", FBO2.getTextureReference(0), 0);
	ofSetColor(255, 255, 255);
	ofDisableAlphaBlending();
	FBO2.draw(0, 0);
	shaderPost.end();
	FBO.end();

	FBO.draw(0, 0);
}