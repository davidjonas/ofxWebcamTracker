#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  tracker.init();
}

//--------------------------------------------------------------
void ofApp::update(){
  ofBackground(100,100,100);
  tracker.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  tracker.drawDebug(0,0); //Draws all the visualizations
  //tracker.drawRGB(0,0);
  //tracker.drawContours(0,0);
  //tracker.drawBlobPositions(0,0);
  //tracker.drawEdgeThreshold(0,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  switch (key){
		case ' ':
			tracker.grabBackground();
			break;
    case 'w':
      tracker.setThreshold(tracker.getThreshold()+1);
      break;
    case 's':
      tracker.setThreshold(tracker.getThreshold()-1);
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
