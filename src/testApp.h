#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxAciveSearchImage.h"

using namespace cv;
using namespace ofxCv;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    float framerate;
    
    ofxActiveSearchImage activeserach, activeserach2;
    ofImage original, ref;
    CvHistogram *hist, *hist2;
    
    int resultxscale, resultyscale;
    double resultscale;
    
    float resultdata;
    
    ofVideoGrabber video;
    int camWidth, camHeight;
    ofImage videoImg;
    cv::Mat cvImgMat;
    ofImage ofImg;

    ofxCv::FlowFarneback farneback;
    ofxCv::FlowPyrLK pyrLk;
    ofxCv::Flow* curFlow;

};
