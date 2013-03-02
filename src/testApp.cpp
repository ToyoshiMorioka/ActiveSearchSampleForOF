#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
//    original.loadImage("original2.jpg");
//    ref.loadImage("button2.jpg");
    original.loadImage("Input.jpg");
    ref.loadImage("Reference.jpg");
    
    activeserach.active_search(original, ref, resultxscale, resultyscale, resultscale);
    
}

//--------------------------------------------------------------
void testApp::update(){
    double tempScale = 1.0;
    framerate = ofGetFrameRate();
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0, 0, 0);
    
    original.draw(0, 0, original.width, original.height);

    ofSetColor(255, 0, 0);
    float thisScale = resultscale;
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofRect(resultxscale / thisScale,
           resultyscale / thisScale ,
           ref.width / thisScale,
           ref.height/ thisScale);


    ofSetColor(255, 255, 255);
    ofDrawBitmapString(ofToString(framerate), 10, 10);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}