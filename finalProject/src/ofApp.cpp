#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundAuto(false);
	ofDisableArbTex();

	//variables
	camHeight = 180;
	camWidth = 320;

	rect_size = 8;
	threshold = 80;
	bLearnBackground = true;

	//camera
	//ofSetFrameRate(60);
	vid_grabber.setVerbose(true);
	vid_grabber.setDeviceID(1);
	vid_grabber.setup(camWidth, camHeight);
	
	//vid_grabber2.setVerbose(true);
	//vid_grabber2.setDeviceID(1);
	//vid_grabber2.setup(camWidth, camHeight);
	
	//flow
	flow.setPyramidScale(0.5);
	flow.setNumLevels(4);
	flow.setWindowSize(32);
	flow.setNumIterations(2);
	flow.setPolyN(7);
	flow.setPolySigma(1.5);
	flow.setUseGaussian(true);
	

	//CV blob
	grayImage.allocate(camWidth,camHeight);
	grayBg.allocate(camWidth, camHeight);
	grayDiff.allocate(camWidth, camHeight);

	ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::update(){	
	vid_grabber.update();
	//vid_grabber2.update();

	if (vid_grabber.isFrameNew()) {
	//Calculate Flow
		flow.calcOpticalFlow(vid_grabber);
		colorImg.setFromPixels(vid_grabber.getPixels());

		grayImage = colorImg;
		if (bLearnBackground == true) {
			grayBg = grayImage;
			bLearnBackground = false;
		}
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);
		contourFinder.findContours(grayDiff, 90, (camWidth*camHeight) / 4, 2, false);
	} //new frame
}

//--------------------------------------------------------------
void ofApp::draw(){
	/*ofSetColor(255, 90);
	ofDrawRectangle(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	*/
	//scale to screen
	ofSetColor(255);
	//grayDiff.draw(0, 0);
	//contourFinder.draw(0,0);
	ofPushMatrix();
	ofTranslate(0, 0);
	ofScale((ofGetWindowWidth())/camWidth,(ofGetWindowHeight())/camHeight);

	//ofLog() << "numblobs:" + ofToString(contourFinder.nBlobs) << endl;
	//ofLog() << "threshold" + ofToString(threshold) << endl;


	//second camera testing

	//Grabbing background
	if (vid_grabber.isFrameNew()) {
		ofPixels & pixels = vid_grabber.getPixels();
		frame.setFromPixels(pixels);
		if (!ff_bool) {
			first_frame = frame;
			ff_bool = true;
		}
	}
	if (ff_bool && !ff_drawn) {
		//vid_grabber.draw(0,0);
		ofSetColor(150, 120, 150, 10);
		first_frame.draw(0,0);
		//colorImg.draw(0, 0);
		//ff_drawn = true;
	}

	///////////Frame subsectioning ///////////

		//cycle through vid_grabber in sections

		for (size_t x = 0; x < camWidth - 8; x += rect_size) {
			for (size_t y = 0; y < camHeight - 8; y += rect_size) {
				
				ofPixels & bwpixels = grayDiff.getPixels();
				
				for (int i = 0; i < contourFinder.nBlobs; i++) {
						ofVec2f thisPos = ofVec2f(x, y);

						if (contourFinder.blobs[i].boundingRect.inside(x, y)) {
							ofRectangle currArea(x, y, rect_size, rect_size);
							ofVec2f areaVec = flow.getAverageFlowInRegion(currArea);
							int mag = abs(areaVec.x) + abs(areaVec.y);
							int dir = atan2(areaVec.y, areaVec.x);

							ofPushMatrix();
							ofTranslate(x, y);
							ofRotateDeg(90);
							if (mag > 3) {
								ofSetColor(90 + (40 * mag * dir), 120+(mag*5), 100 + (150 * mag)/(dir+0.01), 180);
							}
							else {
								ofSetColor(190 + 40 * mag, 190, 100 + 20 * mag, 199);
							}
							
							frame.drawSubsection(0, 0, rect_size, rect_size, x, y);
							ofPopMatrix();

							rects.push_back(thisPos);
						}
						else {
							for (int i = 0; i < rects.size(); i++){
								if (rects[i] == thisPos){
									rects.erase(rects.begin() + i);
								}
							}
						}
				}
				
				//flow calc
				//ofRectangle currArea(x, y, rect_size, rect_size);
				//ofVec2f areaVec = flow.getAverageFlowInRegion(currArea);
				//int mag = abs(areaVec.x) + abs(areaVec.y);
				//int dir = atan2(areaVec.y, areaVec.x);


				//rotation/translation
				
				//ofRotateRad(mag);
					//record sound here// r g b variables? //include direction? rot?
					///////ofSetColor(40*mag, 200, 190*mag, 180);
					/*ofPixels & pixels = frame.getPixels();
					pixels.cropTo(crop, x, y, rect_size, rect_size);
					imgDst.setFromPixels(crop);*/

					//////////3D BEGIN/////////

					//ofPushMatrix();


					//ofScale(1, -1, -1);
					//ofTranslate(0, 0, mag*10.0);
			
					/*imgDst.getTexture().bind();
					plane.set(rect_size,rect_size);
					plane.draw();
					imgDst.getTexture().unbind();
					if (mag > 15) {
						ofTranslate(dir*2, dir*2, dir*2);
						ofSetColor(mag * 100, 0, dir * 90);
						plane2.set(mag, mag);
						//plane2.draw();
					}*/
					//ofPopMatrix();
					//////////////old//////////////
					//second line:
					//imgDst.draw(0, 0);
					//original line:
					/////frame.drawSubsection(0, 0, rect_size, rect_size, x, y);
			}
		}
	ofPopMatrix();
	ofLog() << rects.size() << endl;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key) {
	case ' ':
		bLearnBackground = true;
		//ff_drawn = false;
		first_frame = frame;
		ofLog() << "new bg" << endl;
		break;
	case '+':
		threshold++;
		if (threshold > 255) threshold = 255;
		ofLog() << threshold << endl;

		break;
	case '-':
		threshold--;
		if (threshold < 0) threshold = 0;
		ofLog() << threshold << endl;

		break;
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
