#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  tracker.init();
  tracker.setThreshold(60);
}

//--------------------------------------------------------------
void ofApp::update(){
  ofBackground(100,100,100);
  tracker.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  tracker.drawRGB(0,0,1);
  tracker.drawContours(0,0,1);
  tracker.drawBlobPositions(0,0,1);
  tracker.drawEdgeThreshold(0,0, 1);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  switch (key){
		case ' ':
			tracker.grabBackground();
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
