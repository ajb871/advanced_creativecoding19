#include "ofApp.h"
//--------------------------------------------------------------
void ofApp::setup() {
	ofSetBackgroundAuto(false);
	ofSetFrameRate(60);
	ofFill();
	camera.setup(320, 180);
	vidW = camera.getWidth();
	vidH = camera.getHeight();
	camera.setDesiredFrameRate(30);
	//ofSetWindowShape(320, 180);


	// setup gui
	gui.setup();
	gui.add(flowPyrScale.set("Pyramid Scale", .5, 0, .99));
	gui.add(flowLevels.set("Pyramid Levels", 4, 1, 8));
	gui.add(flowIterations.set("Iterations", 2, 1, 8));
	gui.add(flowPolyN.set("Polynomial size", 7, 5, 10));
	gui.add(flowPolySigma.set("PolySigma", 1.5, 1.1, 2));
	gui.add(flowUseGaussian.set("Use Gaussian", true));
	gui.add(flowWinSize.set("Window size", 32, 4, 64));
	gui.add(loopVid.set("Loop video", false));
	gui.add(vidScale.set("Scale video", 4, 1, 3));
	gui.add(lineWidth.set("Draw line width", 4, 1, 10));

	//curFlow = &flow;

}
//--------------------------------------------------------------
void ofApp::update() {

	camera.update();
	if (camera.isFrameNew()) {

		// Farneback flow parameters
		// more info at: http://docs.opencv.org/3.0-beta/modules/video/doc/motion_analysis_and_object_tracking.html#calcopticalflowfarneback

		flow.setPyramidScale(flowPyrScale);
		// 0.5 is classical image pyramid (scales image * 0.5 each layer)

		flow.setNumLevels(flowLevels); // number of levels of scaling pyramid
			// 1 means only original image is analyzed, no pyramid

		flow.setWindowSize(flowWinSize); // average window size
			// larger means robust to noise + better fast motion, but more blurred motion

		flow.setNumIterations(flowIterations); // # iterations of algorithm at each pyramid level

		flow.setPolyN(flowPolyN);
		// size of pixel neighborhood used for per-pixel polynomial expansion
		// larger means image will be approximated at smooth surfaces, more robust algorithm, more blurred motion

		flow.setPolySigma(flowPolySigma);
		// standard deviation of gaussian used to smooth derivates for polynomial expansion
		// for PolyN of 5, use PolySigma 1.1
		// for PolyN of 7, use PolySigma 1.5

		flow.setUseGaussian(flowUseGaussian);
		// use gaussian filter instead of box filter: slower but more accurate
		// normally use a larger window with this option



		// calculate flow
		flow.calcOpticalFlow(camera);



		// save average flows per frame + per vid

		frameFlows.push_back(flow.getAverageFlow());
		if (frameFlows.size() > 8){
			frameFlows.erase(frameFlows.begin());
		}

		vidFlowTotal += frameFlows.back();
		vidFlowAvg = vidFlowTotal / frameFlows.size();

		// calculate dot position to show average movement

		dotPos += frameFlows.back();
		dotPath.addVertex(dotPos);

		
	}
}
//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(0, 0, 0, 90);
	ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());

	ofFill();
	// draw video and flow field

	ofSetColor(255);
	//ofPushMatrix();
	// make room for gui at left
	ofScale(vidScale, vidScale); // scale to vidScale gui choice
	//gui.draw();

	if (cameraOn) {
		camera.draw(0, 0, vidW, vidH); // draw video at scale
	}

	ofSetLineWidth(1); // draw thin flow field
		// (i.e. don't change flow field's line width with gui lineWidth choice)

	if (flowOn) {
		flow.draw(0, 0, vidW, vidH); // draw flow field at scale
	} 

	//use for loop to go through entire image
	for (int x = 1; x < 15; x++)
	{
		for (int y = 1; y < 18; y++)
		{
			ofRectangle currArea(x*18, y*9, 32, 18);
			ofVec2f areaVec = flow.getAverageFlowInRegion(currArea);
			int mag = abs(areaVec.x) + abs(areaVec.y);
			int dir = atan2(areaVec.y, areaVec.x);
			ofSetLineWidth(5);

			ofPushMatrix();

			ofTranslate(x * 18, y * 9);
			ofRotateRad(dir); //rotation based off dir
			ofFill();
			ofSetColor(ofColor(45 - mag * 40, 0, 30 + mag * 80), ofMap(mag, 1, 20, 50, 120));
			
			ofDrawTriangle(mag * 3, 0, -mag * 3, mag * 2, -mag * 3, -mag * 2);
			//ofDrawRectangle(x * 18, y * 9, ofMap(mag, 1, 20, 2, 50), ofMap(mag, 1, 20, 2, 50));
			
			ofPopMatrix();

			ofNoFill();
			ofSetLineWidth(1);
			ofSetColor(255, ofMap(mag, 1, 20, 50, 120));
			ofDrawCircle(x*18, y*9, ofMap(mag,0,20,0,50));

			//ofLog(OF_LOG_NOTICE, ofToString(mag));
;		}
	}

	for (int i = 0; i < frameFlows.size(); i++) {
		ofSetColor(ofColor(0, 0, ofRandom(78, 255)),90);
		ofFill();
		//Currently drawing from center,
		
		ofSetLineWidth(35); // draw other lines based on gui lineWidth
		ofDrawLine(vidW*0.5, vidH*0.5, vidW*0.5 + frameFlows[i].x * 50, vidH*0.5 + frameFlows[i].y * 50);
		ofSetColor(0);
		ofFill();
		ofDrawCircle(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2, 150);
	}


	/* if (frameFlows.size() > 0){
		 // draw average magnitude and direction of this frame, if we have it

		 ofSetColor(255,0,0);

		 ofDrawLine(vidW*0.5, vidH*0.5, vidW*0.5+ frameFlows.back().x * 50, vidH*0.5+ frameFlows.back().y * 50);
		 // scale motion average * 50 for visibility
	 }

	 ofPopMatrix();
	 //More shape drawing
	 if (frameFlows.size() > 0){

		 ofDrawCircle(vidW*2, vidH*2, vidW*0.5+ frameFlows.back().x * 50, vidH*0.5+ frameFlows.back().y * 50);
		 ofBeginShape();
		 ofVertex(frameFlows.back().x * 50,frameFlows.back().y * 50);
		 ofVertex(215,135);
		 ofVertex(365,25);
		 ofVertex(305,200);
		 ofVertex(250,25);
		 ofEndShape();
	 }
 */
}
//--------------------------------------------------------------
void ofApp::reset() {
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'a') {
		if (!flowOn) {
			flowOn = true;
		}
		else if (flowOn) {
			flowOn = false;
		}
	}
	if (key == 'c') {
		if (!cameraOn) {
			cameraOn = true;
		}
		else if (cameraOn) {
			cameraOn = false;
		}
	}

}
//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}
//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}
//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
}