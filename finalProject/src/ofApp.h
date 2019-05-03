#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"

using namespace cv;
using namespace ofxCv;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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
		void checkInside(ofPoint *polygon, int N, ofPoint p);
		
		// Video
		ofVideoGrabber vid_grabber;
		ofVideoGrabber vid_grabber2;
		ofImage frame;
		ofImage first_frame;
		bool ff_bool;
		bool ff_drawn;

		int camHeight;
		int camWidth;

		//Pixels
		ofPixels pixels_manip;
		ofTexture manip_texture;

		ofPixels crop;
		ofImage imgDst;

		//3D
		ofPlanePrimitive plane;
		ofPlanePrimitive plane2;

		ofPlanePrimitive b_plane;

		ofEasyCam cam;

		//Flow & CV
		FlowFarneback flow;
		Flow* curFlow;

		ofxCvColorImage			colorImg;

		ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;
		ofxCvContourFinder 	contourFinder;


		int threshold;
		bool bLearnBackground;


		//Math
		float rect_size;
		vector<ofVec2f> rects;




};
