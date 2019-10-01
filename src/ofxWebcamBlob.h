#pragma once
#include "ofxOpenCv.h"

class ofxWebcamBlob {
  private:
    float zHint;
    float tolerance;
    bool active;
    float lastSeen;
    bool overlap;

  public:
    int id;
    ofxCvBlob blob;
    ofVec3f direction;
    float speed;

    ofxWebcamBlob(int id, ofxCvBlob blob, float zHint, float tolerance);
    ~ofxWebcamBlob();

    void update(ofxCvBlob blob, float zHint);
    bool intersects(ofxWebcamBlob otherBlob);
    ofRectangle getIntersection(ofxWebcamBlob otherBlob);
    float difference(ofxCvBlob otherBlob, float zHint);
    void setTolerance(float value);
    float getTolerance();
    void draw(float x, float y);
    void setActive(bool value);
    void setOverlap(bool value);
    bool isActive();
    bool isOverlapping();
    float timeSinceLastSeen();
};
