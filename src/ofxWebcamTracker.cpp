#include "ofxWebcamTracker.h"

ofxWebcamTracker::ofxWebcamTracker() {
  initialized = false;
  tolerance = 50;
  edgeThreshold = 10.0f;
}

ofxWebcamTracker::ofxWebcamTracker(int index){
  init(index);
}

ofxWebcamTracker::ofxWebcamTracker(int index, float tolerance){
  init(index);
  this->tolerance = tolerance;
}

ofxWebcamTracker::~ofxWebcamTracker(){
  webcam.close();
}

int ofxWebcamTracker::numWebcamsDetected()
{
  return webcam.numWebcamsDetected();
}

void ofxWebcamTracker::init(int index){
  webcamIndex = index;
  webcam.init();
  webcam.open();

  width = webcam.width;
  height = webcam.height;

  colorImg.allocate(width,height);
  grayscale.allocate(width, height);
  background.allocate(width, height);
  diff.allocate(width, height);
  threshold = 3;  //60
  blurAmount = 9;
  backgroundSubtract = false;
  blur = false;
  initialized = true;
  tolerance = 100;
  removeAfterSeconds = 5;
  idCounter = 0;
  minBlobSize = 100;
}

//Getters and setters
void ofxWebcamTracker::setBackgroundSubtract(bool value){
  backgroundSubtract = value;
}

void ofxWebcamTracker::setBlur(bool value){
  blur = value;
}

void ofxWebcamTracker::setBlurAmount(float value){
  if(value >= 1)
  {
    blurAmount = value;
  }
  else
  {
    blurAmount = 1;
  }
}

void ofxWebcamTracker::setThreshold(float value){
  threshold = value;
  if(threshold > 255) threshold = 255;
  if(threshold < 0) threshold = 0;
}

void ofxWebcamTracker::setTolerance(float value){
  tolerance = value;
  //Updating existing blobs.
  for(uint8_t i=0; i<blobs.size(); i++)
  {
    blobs[i].setTolerance(tolerance);
  }
}

void ofxWebcamTracker::setRemoveAfterSeconds(float value){
  removeAfterSeconds = value;
}

void ofxWebcamTracker::setEdgeThreshold(float value){
  edgeThreshold = value;
}

void ofxWebcamTracker::setMinBlobSize(float value){
  minBlobSize = value;
}

bool ofxWebcamTracker::getBackgroundSubtract(){
  return backgroundSubtract;
}

bool ofxWebcamTracker::getBlur(){
  return blur;
}

float ofxWebcamTracker::getBlurAmount(){
  return blurAmount;
}

float ofxWebcamTracker::getThreshold(){
  return threshold;
}

float ofxWebcamTracker::getTolerance(){
  return tolerance;
}

float ofxWebcamTracker::getRemoveAfterSeconds(){
  return removeAfterSeconds;
}

float ofxWebcamTracker::getEdgeThreshold(){
  return edgeThreshold;
}

float ofxWebcamTracker::getMinBlobSize(){
  return minBlobSize;
}

int ofxWebcamTracker::getWidth(){
  return width;
}

int ofxWebcamTracker::getHeight(){
  return height;
}

int ofxWebcamTracker::getWebcamIndex(){
  return webcamIndex;
}

int ofxWebcamTracker::getNumActiveBlobs()
{
  uint8_t count = 0;
  for(uint8_t i=0; i<blobs.size(); i++)
  {
    if(blobs[i].isActive())
    {
      count++;
    }
  }
  return count;
}

vector<ofxWebcamBlob> ofxWebcamTracker::getActiveBlobs(){
  vector<ofxWebcamBlob> vec;

  for(uint8_t i=0; i<blobs.size(); i++)
  {
    if(blobs[i].isActive())
    {
      vec.push_back(blobs[i]);
    }
  }

  return vec;
}

bool ofxWebcamTracker::isOverlapCandidate(ofxWebcamBlob blob){

  ofRectangle margin(edgeThreshold, edgeThreshold, width-edgeThreshold*2, height-edgeThreshold*2);
  ofRectangle inter = blob.blob.boundingRect.getIntersection(margin);
  return inter.getArea() >= blob.blob.boundingRect.getArea()/2;
}

void ofxWebcamTracker::update(){
  webcam.update();
  colorImg.setFromPixels(webcam.getPixels());
  grayscale = colorImg;

  if(blur)
  {
    grayscale.blurGaussian(blurAmount);
  }

  if(backgroundSubtract){
    subtractBackground();
    contourFinder.findContours(diff, minBlobSize, (width*height)/2, 20, false);
  }
  else {
    contourFinder.findContours(grayscale, minBlobSize, (width*height)/2, 20, false);
  }

  matchAndUpdateBlobs();
}

void ofxWebcamTracker::grabBackground() {
  background.setFromPixels(grayscale.getPixels());
  backgroundSubtract = true;
  clearBlobs();
}

void ofxWebcamTracker::subtractBackground() {

  ofPixels & pix = grayscale.getPixels();
	ofPixels & bgPix = background.getPixels();
	ofPixels & d = diff.getPixels();

  //TODO: Better to do the threshold in the source image
  //like it is done in https://github.com/openframeworks/openFrameworks/blob/master/examples/computer_vision/webcamExample/src/ofApp.cpp

	int numPixels = pix.size();
	for(int i = 0; i < numPixels; i++) {
		if(abs(pix[i] - bgPix[i]) < threshold) {
			d[i] = 0;
		}
		else
		{
			d[i] = 255;
		}
	}

  diff.flagImageChanged();
}

//The Tracker
void ofxWebcamTracker::matchAndUpdateBlobs()
{
  vector<ofxCvBlob> cvBlobs = contourFinder.blobs;
  vector<bool> trackedBlob(blobs.size(), false);
  vector<ofxCvBlob>::iterator currentBlob = cvBlobs.begin();
  vector<ofxCvBlob> newBlobs;

  while(currentBlob != cvBlobs.end())
  {
    int chosenMatch = -1;
    float minDifference = 10000; //TODO: this should be flagged instead of ridiculous value.

    for(int i=0; i<blobs.size(); i++)
    {
      float blobDiff = blobs[i].difference(*currentBlob, getZHintForBlob(*currentBlob));

      if(blobDiff != -1 && blobDiff <= minDifference)
      {
        if(blobDiff == minDifference)
        {
          //TODO: There are two blobs that match, this is really rare. How to decide which blob is which?
          //      right now the latest blob in the vector will be chosen.
          ofLog(OF_LOG_WARNING) << "Blob conflict found!!" << endl;
        }
        minDifference = blobDiff;
        chosenMatch = i;
      }
    }

    if(chosenMatch != -1)
    {
      blobs[chosenMatch].update(*currentBlob, getZHintForBlob(*currentBlob));
      trackedBlob[chosenMatch] = true;
    }
    else
    {
      bool isValid = true;
      // for(int i=0; i<blobs.size(); i++)
      // {
      //   float interArea = currentBlob->boundingRect.getIntersection(blobs[i].blob.boundingRect).getArea();
      //   if(interArea != 0 && (interArea >= currentBlob->boundingRect.getArea() * 0.9 || interArea >= blobs[i].blob.boundingRect.getArea() * 0.7))
      //   {
      //     isValid = false;
      //   }
      // }

      if(isValid) newBlobs.push_back(*currentBlob);
    }

    currentBlob++;
  }

  for(int i=0; i<newBlobs.size(); i++)
  {
    float zHint = getZHintForBlob(newBlobs[i]);
    ofxWebcamBlob newBlob(++idCounter, newBlobs[i], zHint, tolerance);
    blobs.push_back(newBlob);
  }

  for(int i=0; i<trackedBlob.size(); i++)
  {
      if(!trackedBlob[i])
      {
        if(!blobs[i].isOverlapping() && blobs[i].timeSinceLastSeen() > removeAfterSeconds)
        {
            if(i < blobs.size()){
                blobs.erase(blobs.begin()+i);
            }
        }


        if((blobs[i].isActive() || blobs[i].isOverlapping()) && isOverlapCandidate(blobs[i]))
        {
          //TODO: Check for ovelapping blobs;
          int overlapIndex = -1;
          for(uint8_t b=0; b<blobs.size(); b++)
          {
            if(trackedBlob[b] && isOverlapCandidate(blobs[b]) && blobs[i].intersects(blobs[b]))
            {
              //BLOBS OVERLAP!
              blobs[i].setOverlap(true);
              blobs[b].setOverlap(true);
              overlapIndex = b;
              break;
            }
          }

          if(overlapIndex == -1)
          {
            blobs[i].setOverlap(false);
          }
        }
      }
      else {
        if(blobs[i].isOverlapping())
        {
          for(uint8_t b=0; b<blobs.size(); b++)
          {
            if(trackedBlob[b] && blobs[b].isOverlapping() && !blobs[i].intersects(blobs[b]))
            {
              //BLOBS STOPPED OVERLAPPING!
              blobs[i].setOverlap(false);
              blobs[b].setOverlap(false);
              break;
            }
          }
        }
      }
      blobs[i].setActive(trackedBlob[i]);
  }
}

float ofxWebcamTracker::getZHintForBlob(ofxCvBlob blob)
{
  return webcam.getDistanceAt(blob.centroid);
}

void ofxWebcamTracker::clearBlobs(){
  blobs.clear();
}


//Image Getters
ofxCvColorImage ofxWebcamTracker::getColorImage(){
  return colorImg;
}

ofxCvGrayscaleImage ofxWebcamTracker::getGrayImage(){
  return grayscale;
}

ofxCvGrayscaleImage ofxWebcamTracker::getDepthImage(){
  return depthImage;
}

//Draw and debug methods
void ofxWebcamTracker::drawDepth(float x, float y)
{
  depthImage.draw(x, y);
}

void ofxWebcamTracker::drawDepth(float x, float y, float scale)
{
  depthImage.draw(x, y, width*scale, height*scale);
}

void ofxWebcamTracker::drawRGB(float x, float y)
{
  colorImg.draw(x, y, width, height);
}

void ofxWebcamTracker::drawRGB(float x, float y, float scale)
{
  colorImg.draw(x, y, width*scale, height*scale);
}

void ofxWebcamTracker::drawBlobPositions(float x, float y){
  drawBlobPositions(x,y,1.0);
}

void ofxWebcamTracker::drawBlobPositions(float x, float y, float scale){
  for (int i = 0; i < blobs.size(); i++){

    if(blobs[i].isActive())
    {
      ofFill();
      ofSetColor(255,0,0);
      ofCircle(x+ blobs[i].blob.centroid.x * scale,
             y+ blobs[i].blob.centroid.y * scale,
             10);

      ofSetColor(255);
      ofDrawBitmapString(ofToString(blobs[i].id),
            x+ blobs[i].blob.centroid.x * scale,
            y+ blobs[i].blob.centroid.y * scale);

      if(blobs[i].isOverlapping())
      {
        ofSetLineWidth(10);
        ofNoFill();
        ofSetColor(255,0,0);
        ofDrawRectangle(x+blobs[i].blob.boundingRect.x * scale,
                        y+blobs[i].blob.boundingRect.y * scale,
                        blobs[i].blob.boundingRect.width * scale,
                        blobs[i].blob.boundingRect.height * scale);
        ofSetLineWidth(1);
      }
    }
    else {
      ofNoFill();
      ofCircle(x+ blobs[i].blob.centroid.x * scale,
             y+ blobs[i].blob.centroid.y * scale,
             10);

      ofSetColor(255);
      ofDrawBitmapString(ofToString(blobs[i].id),
            x+ blobs[i].blob.centroid.x * scale,
            y+ blobs[i].blob.centroid.y * scale);
    }
  }
}

void ofxWebcamTracker::drawBackground(float x, float y){
  background.draw(x,y);
}

void ofxWebcamTracker::drawBackground(float x, float y, float scale){
  background.draw(x,y,width*scale, height*scale);
}

void ofxWebcamTracker::drawContours(float x, float y){
  contourFinder.draw(x,y);
}

void ofxWebcamTracker::drawContours(float x, float y, float scale){
  contourFinder.draw(x,y,width*scale,height*scale);
}

void ofxWebcamTracker::drawDiff(float x, float y){
  diff.draw(x, y);
}

void ofxWebcamTracker::drawDiff(float x, float y, float scale){
  diff.draw(x,y,width*scale,height*scale);
}

void ofxWebcamTracker::drawDebug(float x, float y) {
  drawDebug(x, y, 1.0);
}


void ofxWebcamTracker::drawDebug(float x, float y, float scale) {
  drawDepth(x,y,0.5 * scale);
  drawBackground(x+width*scale/2, y, 0.5 * scale);
  drawDiff(x, y+height*scale/2, 0.5 * scale);
  drawRGB(x+width*scale/2, y+height*scale/2, 0.5 * scale);
  drawContours(x+width*scale/2, y+height*scale/2, 0.5 * scale);
  drawBlobPositions(x+width*scale/2, y+height*scale/2, 0.5 * scale);
  drawEdgeThreshold(x+width*scale/2, y+height*scale/2, 0.5 * scale);
}

void ofxWebcamTracker::drawEdgeThreshold(float x, float y)
{
  drawEdgeThreshold(x, y, 1.0);
}

void ofxWebcamTracker::drawEdgeThreshold(float x, float y, float scale)
{
  ofNoFill();
  ofSetColor(255, 90, 90);
  ofDrawRectangle(x+(edgeThreshold * scale),y+(edgeThreshold * scale), (width*scale)-((edgeThreshold * scale)*2), (height*scale)-((edgeThreshold * scale)*2));
}

//Calibration
void ofxWebcamTracker::calibratePosition(int index, ofPoint p){
  webcam.calibratePosition(index, p);
}

void ofxWebcamTracker::close(){
  webcam.close();
}