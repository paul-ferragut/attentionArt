#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofEnableAlphaBlending();
	ofBackground(255);

	bounds = ofRectangle(0, 0, ofGetWidth() / 2 , ofGetHeight() / 2 );

	vectorField.setup(bounds.getWidth(),bounds.getHeight(), 5);

	vectorField.randomize();

	int pointCount = 200;
	int seed = 33;

	points = generateRandomPoints(pointCount, seed, bounds);

	for (int i = 0;i < pointCount;i++) {

		Particle newParticle;
		ofVec2f pos(ofRandom(0, bounds.getWidth()),  ofRandom(0, bounds.getHeight()));
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

	gui.setup("GUI", "settings.xml");

	gui.add(colorMapDebug.setup("colorMapDebug",true));
	gui.add(vectorFieldDebug.setup("vectorFieldDebug", true));
	gui.add(centerVoroDebug.setup("centerVoroDebug", true));

	gui.add(scaleVectorfield.setup("scale vectorfield", 1, 0.1, 7));
	gui.add(animateVectorfield.setup("animate vectorfield", 0.5, 0.0,1.0));
	gui.add(blurRadius.setup("blurRadius", 10, 0.0, 20.0));

	gui.add(refreshPixel.setup("refreshPixel", false));
	gui.add(imagePaint.setup("imagePaint", false));
	gui.add(paintDebug.setup("paintDebug", false));

	width = ofGetWidth();
	height = ofGetHeight();
	//imagePaint = false;
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

	string fragShader = STRINGIFY(uniform sampler2DRect backbuffer;
	uniform sampler2DRect normals;
	uniform sampler2DRect dampMap;

	void main() {
		vec2 st = gl_TexCoord[0].st;

		vec4 newFrame = texture2DRect(backbuffer, st);
		vec4 color = vec4(0, 0, 0, 0);
		vec2 norm = (texture2DRect(normals, st).rg - 0.5) * 2.0;
		float damp = texture2DRect(dampMap, st).r;
		float inc = (abs(norm.x) + abs(norm.y)) * 0.5;

		vec2 offset[36];
		int iTotal = 36;
		float fTotal = 36.0;

		float pi = 3.14159265358979323846;
		float step = (pi*2.0) / fTotal;
		float angle = 0.0;
		for (int i = 0; i < iTotal; i++) {
			offset[i].x = cos(angle);
			offset[i].y = sin(angle);
			angle += step;
		}

		float sources = 0.0;
		for (int i = 0; i < iTotal; i++) {
			vec4 goingTo = (texture2DRect(normals, st + offset[i]) - 0.5) * 2.0;

			if (dot(goingTo.rg, offset[i]) < -1.0 / fTotal) {
				sources += 1.0;
				color += texture2DRect(backbuffer, st + offset[i]);
			}
		}

		color = color / sources;
		inc = 1.0 - damp;

		gl_FragColor = color*(1.0 - inc) + newFrame*inc;
	}
	);



	FBO.allocate(width, height, GL_RGB, 8);

	shaderPaint.setupShaderFromSource(GL_FRAGMENT_SHADER, fragShader);
	shaderPaint.linkProgram();

}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle("FPS:" + ofToString(ofGetFrameRate()));

	//vectorField.randomize();
	vectorField.animate(animateVectorfield*0.004);
	vectorField.normalize();
	vectorField.scale(scaleVectorfield);
	vectorField.bias(0, 0);
	vectorField.blur();

	for (int i = 0; i<particles.size(); i++) {
		particles[i].move(vectorField.getVectorInterpolated(particles[i].pos.x, particles[i].pos.y, bounds.getWidth(), bounds.getHeight()));
		particles[i].reset(bounds);
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
		

		//pingpong[1].draw(0, 0);

		//shaderBlend.begin();
		//image.draw(0, 0);
		//cam.draw(0, 0);
		FBO.draw(0, 0);
		//shaderBlend.end();

		pingpong[0].end();

		pingpong[1].begin();
		//ofSetColor(255, gray2);
		//image.draw(0, 0);
		//cam.draw(0, 0);
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
			FBO.draw(0, 0);
			refreshPixel = false;
		}

		shaderPaint.begin();
		shaderPaint.setUniformTexture("normals", normals.getTexture(), 1);
		shaderPaint.setUniformTexture("dampMap", mask.getTexture(), 2);



		ofSetColor(255, 255, 255, 255);
		pingpong[(timer + 1) % 2].draw(0, 0);//either 0 or 1, opposite of [timer % 2]


		shaderPaint.end();
		pingpong[timer % 2].end();
	}

	 mask.begin();
	 ofClear(0,0);
	 mask.end();


}

//--------------------------------------------------------------
void ofApp::draw(){

	//ofRectangle bounds = voronoi.getBounds();
	ofSetLineWidth(0);
	ofNoFill();
	ofSetColor(220);
	ofDrawRectangle(bounds);

	for (int i = 0; i < points.size(); i++) {
		/*
		if (ofDist(points[i].x, points[i].y, mouseX, mouseY) < 40) {
			ofVec2f dif(points[i].x - mouseX, points[i].y - mouseY);
			dif *= -0.01;
			points[i].set(points[i] + dif);
		}
		*/
		points[i] = particles[i].pos;

	}

	voronoi.setPoints(points);
	voronoi.generate();

	FBO.begin();
	vector <ofxVoronoiCell> cells = voronoi.getCells();
	for (int i = 0; i<cells.size(); i++) {
		
		ofSetColor(0);
		ofColor gray= vectorField.noiseValAtPos(cells[i].pt);

		//ofNoFill();
		ofMesh mesh;
		//
		mesh.setMode(OF_PRIMITIVE_TRIANGLES);
			
		//CRYSTAL LIKE
		/*
		for (int j = 0; j < cells[i].pts.size(); j++) {
			mesh.addVertex(cells[i].pts[j]);
			mesh.addColor(vectorField.noiseValAtPos(cells[i].pts[0]));
			if (j == cells[i].pts.size() - 1) {
				mesh.addVertex(cells[i].pts[0]);
			}
			else {
				mesh.addVertex(cells[i].pts[j+1]);
			}
			mesh.addColor(vectorField.noiseValAtPos(cells[i].pts[0]));
			mesh.addVertex(cells[i].pt);
			mesh.addColor(vectorField.noiseValAtPos(cells[i].pt));
		}
		*/
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
		mesh.draw();
		ofPopMatrix();
		
		/*
		mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
		
		mesh.addColor(vectorField.noiseValAtPos(cells[i].pts[0]));
		for (int j = 0; j < cells[i].pts.size(); j++) {
			
			mesh.addVertex(cells[i].pts[j]);
			mesh.addColor(vectorField.noiseValAtPos(cells[i].pts[0]));
		}
		mesh.addVertex(cells[i].pts[0]);
		mesh.addColor(vectorField.noiseValAtPos(cells[i].pts[0]));

		mesh.draw();
		*/


		/*
		mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
		mesh.addVertices(cells[i].pts);

		//for (int i = 0;i < cells[i].pts.size();i++) {
			//mesh.addColor(col1);
		//}
		
		mesh.draw();
		mesh.clear();
		for (int j = 0; j < cells[i].pts.size(); j++) {
			mesh.addVertex(cells[i].pt);
			mesh.addVertex(cells[i].pts[j]);
		}

		ofSetColor(120);
		mesh.draw();// CRASHING
		*/
		// Draw cell points
		//ofSetColor(ofColor::fromHsb(255. * i / cells.size(), 255., 255.));

		//ofFill();
		//ofDrawCircle(cells[i].pt, 2);

		
	}
	FBO.end();

	

	ofSetColor(255, 255, 255,255);


	FBO.draw(0,0);
	ofSetColor(255, 255, 255, 255);
	pingpong[timer % 2].draw(0, 0);//,ofGetWidth(),ofGetHeight());
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

	ofSetColor(20, 20, 20);
	ofFill();

	if(centerVoroDebug){
		for (int i = 0; i<particles.size(); i++) {
			particles[i].draw();
		}
	}

	if(vectorFieldDebug){
		ofSetColor(20, 20, 20);
		vectorField.draw();
	}

	gui.draw();



}

void ofApp::exit()
{
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
