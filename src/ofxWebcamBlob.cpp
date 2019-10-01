#include "ofxWebcamBlob.h"

ofxWebcamBlob::ofxWebcamBlob(int id, ofxCvBlob blob, float zHint, float tolerance){
  this->blob = blob;
  this->zHint = zHint;
  this->active = true;
  this->tolerance = tolerance;
  this->id = id;
  this->overlap = false;
  speed = 0;
}

ofxWebcamBlob::~ofxWebcamBlob(){

}

void ofxWebcamBlob::update(ofxCvBlob blob, float zHint)
{
  direction.x = blob.centroid.x - this->blob.centroid.x;
  direction.y = blob.centroid.y - this->blob.centroid.y;
  direction.z = zHint - this->zHint;

  speed = direction.length();

  this->blob = blob;
  this->zHint = zHint;
  this->active = true;
}

bool ofxWebcamBlob::intersects(const ofxWebcamBlob otherBlob){
  ofRectangle intersection = blob.boundingRect.getIntersection(otherBlob.blob.boundingRect);
  return intersection.width != 0 || intersection.height != 0 || intersection.x != 0 || intersection.y != 0;
}

ofRectangle ofxWebcamBlob::getIntersection(ofxWebcamBlob otherBlob) {
  return blob.boundingRect.getIntersection(otherBlob.blob.boundingRect);
}

float ofxWebcamBlob::difference(ofxCvBlob otherBlob, float zHint){
  ofVec3f diff(
            otherBlob.centroid.x - blob.centroid.x,
            otherBlob.centroid.y - blob.centroid.y,
            zHint - this->zHint
  );

  float distance = diff.length();

  if (distance > tolerance)
  {
    return -1;
  }

  ofVec3f expectedLocationDiff = (blob.centroid + direction) - otherBlob.centroid;
  float deviation = expectedLocationDiff.length();
  float areaDiff = abs(otherBlob.area - blob.area);

  float result = distance + deviation/5 + areaDiff;

  return result;
}

void ofxWebcamBlob::setTolerance(float value)
{
  tolerance = value;
}

float ofxWebcamBlob::getTolerance()
{
  return tolerance;
}

void ofxWebcamBlob::draw(float x, float y){
  if(active)
  {
    blob.draw(x,y);
  }
}

void ofxWebcamBlob::setActive(bool value)
{
  active = value;
  if(active)
  {
    lastSeen = ofGetElapsedTimef();
  }
}

void ofxWebcamBlob::setOverlap(bool value)
{
  overlap = value;
}

bool ofxWebcamBlob::isActive()
{
  return active;
}

bool ofxWebcamBlob::isOverlapping()
{
  return overlap;
}

float ofxWebcamBlob::timeSinceLastSeen()
{
  return ofGetElapsedTimef() - lastSeen;
}
