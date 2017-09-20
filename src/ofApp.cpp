#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofEnableAlphaBlending();
	ofBackground(255);
	refreshExpB = false;
	//0 IS CALM
	//GUI1
	gui1.setup("Design", "settings1.xml",-110,0);
	gui1.add(setScenario.setup("setScenario", false));
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
	gui1.add(useMasking.setup("use Masking n p s", false));

	gui1.add(debugDistort.setup("debug distort", false));
	for (int i = 0;i < 3;i++) {
		gui1.add(val[i].setup("val" + ofToString(i), 0.0, 0.0, 1.0));
	}


	gui1.loadFromFile("settings1.xml");
	relaxVoronoi = false;
	
	if (useUDPRead) {
		setupUdp();
	}
	else {
		UDPread = 0;
	}

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
		colorRelaxPrev[i] = colorRelax[relaxPaletteNum][i];
		colorRelaxNext[i] = colorRelax[ofRandom(NUM_PALETTES)][i];
	}
	for (int i = 0;i < 3;i++) {
		positionColorRelaxCurrent[i] = colorPosRelax[relaxPaletteNum][i];
		colorPosRelaxNext[i] = colorPosRelax[relaxPaletteNum][i];
		colorPosRelaxPrev[i] = colorPosRelax[relaxPaletteNum][i];
	}
	for (int i = 0;i < 5;i++) {
		colorStressCurrent[i] = colorStress[stressPaletteNum][i];
		colorStressPrev[i] = colorStress[stressPaletteNum][i];
		colorStressNext[i] = colorStress[ofRandom(NUM_PALETTES)][i];
	}
	for (int i = 0;i < 3;i++) {
		positionColorStressCurrent[i] = colorPosStress[stressPaletteNum][i];
		colorPosStressNext[i] = colorPosStress[stressPaletteNum][i];
		colorPosStressPrev[i] = colorPosStress[stressPaletteNum][i];
	}


	//GUI2
	gui3.setup("Interactivity", "settings3.xml", ofGetWidth() - 110, 500);
	gui3.add(useMainVarGSR.setup("use Main Var GSR", false));
	gui3.add(mainVarGSR.setup("main Var GSR", 0.0, 0.0, 100.0));
	gui3.add(mainVarGSRAveraged.setup("main Var GSR averaged", 0.0, 0.0, 100.0));
	
	gui3.add(useUDPRead.setup("use UDP read",false));

	gui3.add(beat.setup("beat", false));
	gui3.add(averaging.setup("averaging main var", 10, 2, 300));
	
	gui3.add(relaxThreshold.setup("relax Threshold", 20, 1, 100));
	gui3.add(stressThreshold.setup("stress Threshold", 80, 0, 100));

	gui3.add(relaxStateTrigger.setup("relax state trigger", 10, 1, 600));
	gui3.add(stressStateTrigger.setup("stress state trigger", 10, 1, 600));
	gui3.add(idleStateTrigger.setup("idle state trigger", 100, 1, 1600));
	gui3.add(usePreset.setup("use preset", false));
	gui3.add(restartPreset.setup("restart preset", false));
	gui3.add(weightPreset.setup("weight preset", 25, 0, 49));
	gui3.add(weightUDPRead.setup("weight UDP Read", 25, 0, 49));
	gui3.add(udpAveraging.setup("averaging UDP", 400, 0, 3000));

	gui3.loadFromFile("settings3.xml");

	lowReadings = false;
	scenarioPathString = "";
	if (setScenario) {
		//ofSetWindowPosition(1920, 0);
		
		loadPreset("scenario2stress.xml");
		//scenarioPathString = ;

	}
	else {
		
		loadPreset("scenario1calm.xml");
		//scenarioPathString = "scenario1.xml";
	}

	relaxCounter = 0;
	stressCounter = 0;
	idleCounter = 0;

	timerGSR = 0;

	trail.resize(3);
	yGraph.resize(3);
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
	//inverse.allocate(width, height);
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





	string path = "/shapes/";
	ofDirectory dir(path);
	//only show png files
	dir.allowExt("xml");
	//populate the directory object
	dir.listDir();


	shapeCounter = dir.size();
	cout << shapeCounter << endl;

	for (int i = 0; i < dir.size(); i++) {
		ofXml nXML;
		vector < ofVec2f>newMaskPts;
		nXML.load("shapes/shape" + ofToString(i) + ".xml");
		if (nXML.exists("STROKE"))
		{
			// This gets the first stroke (notice the [0], it's just like an array)
			nXML.setTo("STROKE[0]");


			do {
				// set our "current" PT to the first one
				if (nXML.getName() == "STROKE" && nXML.setTo("PT[0]"))
				{
					// get each individual x,y for each point
					do {
						int x = nXML.getValue<int>("X");
						int y = nXML.getValue<int>("Y");
						ofVec2f v(x, y);
						cout << v << endl;
						newMaskPts.push_back(v);
					} while (nXML.setToSibling()); // go the next PT

												   // go back up
					nXML.setToParent();
				}

			} while (nXML.setToSibling()); // go to the next STROKE
		}
		maskPts.push_back(newMaskPts);
	}

}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle("FPS:" + ofToString(ofGetFrameRate()));


	if (setScenario) {
		//ofSetWindowPosition(1920, 0);

		loadPreset("scenario2stress.xml");
		//scenarioPathString = ;

	}
	else {

		loadPreset("scenario1calm.xml");
		//scenarioPathString = "scenario1.xml";
	}

	if (useMainVarGSR) {
		 
		if (restartPreset) {
			usePreset = true;
			restartPresetTimeLine();
			interactionState = INBETWEEN;
			restartPreset = false;
			idleCounter = 0;
		}

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


		//IDLE STATE
		if (mainVarGSRAveraged < prevMainVarGSRAveraged + 1.0 && mainVarGSRAveraged > prevMainVarGSRAveraged - 1.0 ) {
			idleCounter++;
			if (idleCounter > idleStateTrigger) {
				idleCounter = idleStateTrigger;
				interactionState = IDLE;
				inB = false;
			}
		}
		else {
			idleCounter = 0;
		}

		/*
		if (lowReadings) {
			interactionState = IDLE;
			lowReadings = false;
	}
		*/

		prevMainVarGSRAveraged = mainVarGSRAveraged;


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
				//refreshPixel = false;

				for (int i = 0;i < 5;i++) {
				colLastIdle[i]=color[i];
				}
				for (int i = 0;i < 3;i++) {
					posLastIdle[i] = positionColor[i];
				}
				for (int i = 0;i < 3;i++) {
					valLastIdle[i] = val[i];
				}
				animateLastIdle = animateVectorfield;
				scaleVecLastIdle = scaleVectorfield;

				idleTransitionCounter = 0;
				//cout << "new IDLE State" << endl;
			}

			if (interactionState == STRESS) {
				relaxVoronoi = false;
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
			if (prevInteractionState == IDLE) { //&& interactionState==INBETWEEN STRESS IDLE
				//timer= ofGetElapsedTimeMillis();
				lastTimeMeasured = ofGetElapsedTimeMillis();
				//restartPreset=true;
			}
		}
		prevInteractionState = interactionState;

		if (interactionState != IDLE) {
		//CONSTANT UPDATING

			//all this in inbetween
			if (interactionState == INBETWEEN) {
				int changeSpeedCol = 300;
				int counterTransitionWithin = ofGetFrameNum() % changeSpeedCol;
				if (counterTransitionWithin == 0) {
					//cout << "counterTransitionWithin" << endl;
					int nextPaletteNum1 = ofRandom(NUM_PALETTES);
					int nextPaletteNum2 = ofRandom(NUM_PALETTES);
					for (int i = 0;i < 5;i++) {
						colorRelaxPrev[i] = colorRelaxCurrent[i];
						colorStressPrev[i] = colorStressCurrent[i];

						colorStressNext[i] = colorStress[nextPaletteNum1][i];
						colorRelaxNext[i] = colorRelax[nextPaletteNum2][i];

					}
					for (int i = 0;i < 3;i++) {
						colorPosRelaxPrev[i] = positionColorRelaxCurrent[i];
						colorPosStressPrev[i] = positionColorStressCurrent[i];
						colorPosStressNext[i] = colorPosStress[nextPaletteNum1][i];
						colorPosRelaxNext[i] = colorPosRelax[nextPaletteNum2][i];
					}
				}
				float mapT = ofMap(counterTransitionWithin, 0, changeSpeedCol, 0, 1.0);
				for (int i = 0;i < 5;i++) {
					float redN = ofLerp(colorRelaxPrev[i].r, colorRelaxNext[i].r, mapT);
					float greenN = ofLerp(colorRelaxPrev[i].g, colorRelaxNext[i].g, mapT);
					float blueN = ofLerp(colorRelaxPrev[i].b, colorRelaxNext[i].b, mapT);
					colorRelaxCurrent[i] = ofColor(redN, greenN, blueN);
				}
				for (int i = 0;i < 5;i++) {
					float redN = ofLerp(colorStressPrev[i].r, colorStressNext[i].r, mapT);
					float greenN = ofLerp(colorStressPrev[i].g, colorStressNext[i].g, mapT);
					float blueN = ofLerp(colorStressPrev[i].b, colorStressNext[i].b, mapT);
					colorStressCurrent[i] = ofColor(redN, greenN, blueN);
				}

				for (int i = 0;i < 3;i++) {
					positionColorRelaxCurrent[i] = ofLerp(colorPosRelaxPrev[i], colorPosRelaxNext[i], mapT);
					positionColorStressCurrent[i] = ofLerp(colorPosStressPrev[i], colorPosStressNext[i], mapT);
				}
			}

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

			//reset timer after duration //maybe trigger idle or pause the update? THIS IS IN IDLE
			if (timerGSR >= DURATION ) {
					lastTimeMeasured = ofGetElapsedTimeMillis();
					//usePreset = false; //????????????
					//usePreset = false;
					//restartPresetTimeLine();
			}
			else {
				timerGSR = ofGetElapsedTimeMillis() - lastTimeMeasured;
				currentPresetVal = getPresetVal(ofMap(timerGSR, 0, DURATION, 0.0, 1.0));
				float flippedWeightPreset = ofMap(weightPreset, 0, 49, 49, 0);
				//cout << "weightPreset" << flippedWeightPreset << endl;
				//cout << "preset val" << currentPresetVal << endl;
				currentPresetVal = ofMap(currentPresetVal, 30, 270, flippedWeightPreset, 100 - flippedWeightPreset,true);
				//cout << "preset val modif" << currentPresetVal << endl;
				//
				if (usePreset) {
					mainVarGSR = currentPresetVal;
				}
				
			}


	
		}
		else {
		//TO IDLE UPDATE
		
			//float normVal = ofMap(mainVarGSRAveraged, relaxThreshold, stressThreshold, 0.0, 1.00, true);
			idleTransitionCounter += 0.004;
			if (idleTransitionCounter > 1.0) {
				idleTransitionCounter = 1.0;
			}

			for (int i = 0;i < 5;i++) {
				float redN = ofLerp(colLastIdle[i].r,i*50, idleTransitionCounter);
				float greenN = ofLerp(colLastIdle[i].g, i * 50, idleTransitionCounter);
				float blueN = ofLerp(colLastIdle[i].b, i * 50, idleTransitionCounter);
				color[i] = ofColor(redN, greenN, blueN);
			}
			
			//for (int i = 0; i < 3; i++) {
				positionColor[0] = ofLerp(posLastIdle[0],0.25,idleTransitionCounter);
				positionColor[1] = ofLerp(posLastIdle[1], 0.5, idleTransitionCounter);
				positionColor[2] = ofLerp(posLastIdle[2], 0.75, idleTransitionCounter);
			//}
				for (int i = 0; i < 3; i++) {
					val[i] = ofLerp(valLastIdle[i],0.0, idleTransitionCounter);
				}

				scaleVectorfield = ofLerp(scaleVecLastIdle, 0.5, idleTransitionCounter);
				animateVectorfield = ofLerp(animateLastIdle, 0.5, idleTransitionCounter);
		}


		if(useUDPRead)
		updateUdp();
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

	mask.setTexture(grayscale.getTexture(), 1);//IMPORTANT
	mask.update();
	blur.setRadius(blurRadius);

	ofSetColor(255, 255);

	if (imagePaint) {
		grayscale.setTexture(FBO.getTextureReference());
		grayscale.update();
		//inverse << grayscale;
		//inverse.update();
		blur << grayscale;
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


	// mask.begin();
	// ofClear(0,0);
	// mask.end();




	 t = ofGetElapsedTimef();
	 if (trail[0].size() == 0) {
		 initTime = t;
	 }
	 t = t - initTime;
	 int speed = 100;
	 xGraph = int(t*speed) % ((ofGetWidth()/2));
	 yGraph[0] = mainVarGSRAveraged;

	 yGraph[1] = mainVarGSR;
	 
	 if(useUDPRead){
		 yGraph[2] = UDPread;
	}

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

	ofClear(0,0);

	drawWithPost();

	ofSetColor(255, 255, 255, opacityPaint);
	pingpong[timer % 2].draw(0, 0);//,ofGetWidth(),ofGetHeight());


	if (useMasking) {

		//---------
		//Lets draw the stroke as a continous line
		ofSetColor(0, 0, 0);
		ofFill();

		for (int i = 0; i < maskPts.size(); i++) {
			if (maskPts[i].size() > 0) {
				ofBeginShape();
				for (int j = 0; j < maskPts[i].size(); j++) {
					ofVertex(maskPts[i][j].x, maskPts[i][j].y);
					cout << maskPts[i][j].x << endl;
					//line.addVertex(dragPts[i].x, dragPts[i].y);
				}
				ofEndShape();
			}
		}

		if (lastMaskPts.size() > 0) {
			ofBeginShape();
			for (int j = 0; j < lastMaskPts.size(); j++) {
				ofVertex(lastMaskPts[j].x, lastMaskPts[j].y);
			}
			ofEndShape();
		}

	}




	if(guiVisible)
	drawDebug();
	ofFill();
	ofSetColor(255,255);


	if (refreshExpB) {
		refreshExpCol-=10;
		if (refreshExpCol <= 0) {
			refreshExpCol = 0;
			refreshExpB = false;
		}
		cout << refreshExpCol << endl;
		ofEnableAlphaBlending();
		ofSetColor(refreshExpCol, refreshExpCol, refreshExpCol, refreshExpCol);
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
		//ofColor(255, 255);	
	}
		
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
		
		if (i == 1) {
		ofDrawBitmapString("State: " + stateString[interactionState]+" progress: "+ofToString(ofMap(timerGSR, 0, DURATION, 0.0, 1.0))+" file:"+scenarioPathString, 15, 15);
		}

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

	udpConnection.Close();
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
	//	screenGrab = !screenGrab;
	//	ofSaveScreen(ofGetTimestampString() + ".png");
	}



	if (key == 'n')
	{
		//lets clear everything on mouse pressed so we save just one stroke. 
		lastMaskPts.clear();
		XML.clear();
		XML.addChild("DRAWING");

		// let's go back to the root (this is the same thing as reset() btw)
		XML.setTo("//DRAWING");

		// add a new stroke and then figure out how many strokes there are
		XML.addChild("STROKE");
		int strokes = XML.getNumChildren("STROKE");

		// use a compound path: STROKE[2] for example to set the current element
		// to the 3rd stroke tag
		XML.setTo("STROKE[" + ofToString(strokes - 1) + "]");
		xmlStructure = "<STROKE>\n";
	}


	if (key == 'p')
	{

		ofXml point;
		point.addChild("PT");
		point.setTo("PT");

		point.addValue("X", mouseX);
		point.addValue("Y", mouseY);

		XML.addXml(point);
		ofVec2f v(mouseX, mouseY);
		lastMaskPts.push_back(v);
	}

	//no data gets saved unless you hit the s key
	if (key == 's')
	{
		//update the colors to the XML structure when the mouse is released
		XML.setTo("//DRAWING"); // set back to the root node
		XML.save("shapes/shape" + ofToString(shapeCounter) + ".xml");



		maskPts.push_back(lastMaskPts);
	}


	if (key == 'r')
	{
		restartPreset = true;
		refreshExpB = true;
		refreshExpCol = 255;
	}

	if (key == 'i')
	{
		//lowReadings = true;
		//interactionState = IDLE;
		//interactionState = IDLE;
		usePreset = false;
		idleCounter = idleStateTrigger+100;
		interactionState = IDLE;
	


		refreshExpB = true;
		refreshExpCol = 255;
		//inB = false;
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

void ofApp::loadPreset(string presetString) {

	
	if (presetString != scenarioPathString) {
	cout << "trying SET SCENARIO" << endl;
	ofXml XMLsc;

	string message;

	vector<ofVec2f> dragPts;

	if (XMLsc.load(presetString)) {
		message = presetString+" loaded!";
		cout << message << endl;
	}
	else {

		// Ok, we didn't get a file, but that's ok, we're going
		// to go ahead and start making some stuff anyways! First, to
		// make a correct XML document, we need a root element, so
		// let's do that:

	//	XMLsc.addChild("DRAWING");

		// now we set our current element so we're on the right
		// element, so when we add new nodes, they're still inside
		// the root element
	//	XMLsc.setTo("DRAWING");

		message = "unable to load mySettings.xml check data/ folder";
		cout << message << endl;
	}



	// If we have STROKE nodes that we've already created, then we can go ahead and
	// load them into the dragPts so they're drawn to the screen
	if (XMLsc.exists("STROKE"))
	{
		// This gets the first stroke (notice the [0], it's just like an array)
		XMLsc.setTo("STROKE[0]");


		do {
			// set our "current" PT to the first one
			if (XMLsc.getName() == "STROKE" && XMLsc.setTo("PT[0]"))
			{
				// get each individual x,y for each point
				do {
					int x = XMLsc.getValue<int>("X");
					int y = XMLsc.getValue<int>("Y");
					ofVec2f v(x, y);
				//	cout << v << endl;
					dragPts.push_back(v);
				} while (XMLsc.setToSibling()); // go the next PT

											 // go back up
				XMLsc.setToParent();
			}

		} while (XMLsc.setToSibling()); // go to the next STROKE
	}

	for (int i = 0; i < dragPts.size(); i++) {
		ofVertex(dragPts[i].x, dragPts[i].y);
		line.addVertex(dragPts[i].x, dragPts[i].y);
	}

	ofSetColor(255, 255, 0);
	line = line.getSmoothed(2);
	line.draw();

	//ofFill();
	//ofDrawCircle(line.getPointAtPercent(ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 1.0)), 15);
	scenarioPathString = presetString;
	}

}

float ofApp::getPresetVal(float percent) {

	return line.getPointAtPercent(percent).y;
}

void ofApp::restartPresetTimeLine()
{
	
	//timerGSR = ofGetElapsedTimeMillis() - lastTimeMeasured;
	lastTimeMeasured = ofGetElapsedTimeMillis();
	timerGSR = ofGetElapsedTimeMillis() - lastTimeMeasured;
}


void ofApp::setupUdp() {
	udpConnection.Create();

	if (setScenario) {
		udpConnection.Bind(61559);
	}else{
		udpConnection.Bind(61557);
	}
	
	udpConnection.SetNonBlocking(true);
	
	cout << "SETUP UDP" << endl;
}

void ofApp::updateUdp()
{
	char udpMessage[100000];
	udpConnection.Receive(udpMessage, 100000);
	string message = udpMessage;
	if (message != "") {
		/*cout << message << endl;
		stroke.clear();
		float x, y;
		vector<string> strPoints = ofSplitString(message, "[/p]");
		for (unsigned int i = 0;i<strPoints.size();i++) {
			vector<string> point = ofSplitString(strPoints[i], "|");
			if (point.size() == 2) {
				x = atof(point[0].c_str());
				y = atof(point[1].c_str());
				stroke.push_back(ofPoint(x, y));
			}
		}*/
		
		//vector<string>splitString = ofSplitString(message,",",true,true);
		UDPread = ofToFloat(message);



	if (udpHistory.size() >= 500) {
		udpHistory.push_back(UDPread);
		udpHistory.erase(udpHistory.begin());
	}
	else {
		udpHistory.push_back(UDPread);
	}

	double max = *max_element(udpHistory.begin(), udpHistory.end());
	//cout << "Max value: " << max << endl;

	double min = *min_element(udpHistory.begin(), udpHistory.end());
	//cout << "Min value: " << max << endl;
	
	//CALCULATE THE RECENT AVERAGE
	if (udpHistory.size() > udpAveraging) {
		float udpAverage = 0;
		for (int i = udpHistory.size()- udpAveraging;i < udpHistory.size();i++) {
			udpAverage += udpHistory[i];
		}
		udpAverage=udpAverage / udpAveraging;
		if (udpAverage < 1000 && interactionState != IDLE ) {
			//lowReadings = true;
		}

	}
	

	UDPread = ofMap(UDPread,min,max,0+weightUDPRead,100-weightUDPRead);
mainVarGSR = ((mainVarGSR*3) + UDPread) /4; //AVERAGE WITH 3 QUARTER MORE WEIGHT ON SCENARIO

	}
	else {
	
	}

	


	//return UDPread;

}
