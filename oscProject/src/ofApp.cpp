#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowTitle("oscReceiveExample");
	ofSetFrameRate(60); // run at 60 fps
	ofSetVerticalSync(true);

	// listen on the given port
	ofLog() << "listening for osc messages on port " << PORT;
	receiver.setup(PORT);
	receiver2.setup(PORT2);
	ofSetBackgroundAuto(false);
	posX = ofGetWindowWidth() / 2;
	posY = ofGetWindowHeight() / 2; 
	posX2 = ofGetWindowWidth() / 2;
	posY2 = ofGetWindowHeight() / 2;

}

//--------------------------------------------------------------
void ofApp::update(){
	//Clean up this disaster (ARRAYS & FORLOOPS)
	//Add screenshot button from receivers
	//Add color customization from osc app
	//

	// hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msgStrings[i] = "";
		}
	}

	// check for waiting messages
	while(receiver.hasWaitingMessages()){

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);

		// check for mouse moved message
		if(m.getAddress() == "/mouse/position"){

			// both the arguments are floats
			mouseXf = m.getArgAsFloat(0);
			mouseYf = m.getArgAsFloat(1);
		}
		// check for mouse button message
		else if(m.getAddress() == "/mouse/button"){

			// first argument is int32, second is a string
			mouseButtonInt = m.getArgAsInt32(0);
			mouseButtonState = m.getArgAsString(1);
		}
		// check for an image being sent
		// note: the size of the image depends greatly on your network buffer
		// sizes, if an image is too big the message won't come through
		else if(m.getAddress() == "/image"){
			ofBuffer buffer = m.getArgAsBlob(0);
			receivedImage.load(buffer);
		}
		else if (m.getAddress() == "/accxyz") {
			x = m.getArgAsFloat(0);
			y = m.getArgAsFloat(1);
			z = m.getArgAsFloat(2);
			//1
			//2
		}
		else{

			// unrecognized message: display on the bottom of the screen
			string msgString;
			msgString = m.getAddress();
			msgString += ":";
			for(size_t i = 0; i < m.getNumArgs(); i++){

				// get the argument type
				msgString += " ";
				msgString += m.getArgTypeName(i);
				msgString += ":";

				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msgString += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msgString += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msgString += m.getArgAsString(i);
				}
				else{
					msgString += "unhandled argument type " + m.getArgTypeName(i);
				}
			}

			// add to the list of strings to display
			msgStrings[currentMsgString] = msgString;
			timers[currentMsgString] = ofGetElapsedTimef() + 5.0f;
			currentMsgString = (currentMsgString + 1) % NUM_MSG_STRINGS;

			// clear the next line
			msgStrings[currentMsgString] = "";
		}
	}
	posX += x*2;
	posY += y*2;
	r = 10.0 + (z*5);

	posX = ofClamp(posX, 0, ofGetWindowWidth());
	posY = ofClamp(posY, 0, ofGetWindowHeight());
	r = ofClamp(r, 0, 20.0);


	//////////////////
	// hide old messages

	// check for waiting messages
	while (receiver2.hasWaitingMessages()) {

		// get the next message
		ofxOscMessage m;
		receiver2.getNextMessage(m);

		// check for mouse moved message
		if (m.getAddress() == "/accxyz") {
			x2 = m.getArgAsFloat(0);
			y2 = m.getArgAsFloat(1);
			z2 = m.getArgAsFloat(2);
		}
		else {

			// unrecognized message: display on the bottom of the screen
			string msgString;
			msgString = m.getAddress();
			msgString += ":";
			for (size_t i = 0; i < m.getNumArgs(); i++) {

				// get the argument type
				msgString += " ";
				msgString += m.getArgTypeName(i);
				msgString += ":";

				// display the argument - make sure we get the right type
				if (m.getArgType(i) == OFXOSC_TYPE_INT32) {
					msgString += ofToString(m.getArgAsInt32(i));
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_FLOAT) {
					msgString += ofToString(m.getArgAsFloat(i));
				}
				else if (m.getArgType(i) == OFXOSC_TYPE_STRING) {
					msgString += m.getArgAsString(i);
				}
				else {
					msgString += "unhandled argument type " + m.getArgTypeName(i);
				}
			}

			// add to the list of strings to display
			msgStrings[currentMsgString] = msgString;
			timers[currentMsgString] = ofGetElapsedTimef() + 5.0f;
			currentMsgString = (currentMsgString + 1) % NUM_MSG_STRINGS;

			// clear the next line
			msgStrings[currentMsgString] = "";
		}
	}

	posX2 += x2 * 2;
	posY2 += y2 * 2;
	r2 = 10.0 + (z2 * 5);

	posX2 = ofClamp(posX2, 0, ofGetWindowWidth());
	posY2 = ofClamp(posY2, 0, ofGetWindowHeight());
	r2 = ofClamp(r2, 0, 20.0);

}


//--------------------------------------------------------------
void ofApp::draw(){

	//ofBackgroundGradient(100, 0);

	// if image exists, draw it
	if(receivedImage.getWidth() > 0){
		ofSetColor(255);
		receivedImage.draw(ofGetWidth()/2 - receivedImage.getWidth()/2,
		                   ofGetHeight()/2 - receivedImage.getHeight()/2);
	}

	// draw recent unrecognized messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		ofDrawBitmapStringHighlight(msgStrings[i], 10, 40 + 15 * i);
	}

	string buf = "listening for osc messages on port " + ofToString(PORT);
	ofDrawBitmapStringHighlight(buf, 10, 20);

	// draw mouse state
	ofPoint mouseIn(mouseXf*ofGetWidth(), mouseYf*ofGetHeight());
	if(mouseButtonInt == 0){
		ofSetColor(255);
	} else{
		ofSetColor(ofColor::salmon);
	}
	ofDrawCircle(mouseIn, 20);
	ofDrawBitmapStringHighlight(mouseButtonState, mouseIn);
	ofSetColor(x*80 + 90, 20, z * 100);
	ofDrawCircle(posX,posY,r);
	ofDrawCircle(posX2, posY2, r2);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
