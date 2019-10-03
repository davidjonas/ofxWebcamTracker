#pragma once

#include "ofxOpenCv.h"
#include "ofxWebcamBlob.h"
#include "ofxWebcamArray.h"

class ofxWebcamTracker {
  private:
    ofxWebcamArray webcam;
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayscale;
    ofxCvGrayscaleImage background;
    ofxCvGrayscaleImage diff;
    ofxCvContourFinder contourFinder;

    //flags
    bool backgroundSubtract;
    bool outdoorMode;
    bool blur;
    bool initialized;

    float blurAmount;
    float threshold;
    float width;
    float height;
    float maxBlobs;
    float tolerance;
    float removeAfterSeconds;
    int idCounter;
    float minBlobSize;
    float edgeThreshold;
    float lastBackgroundGrab;

  public:
    vector<ofxWebcamBlob> blobs;

    ofxWebcamTracker();
    ~ofxWebcamTracker();

    int numWebcamsDetected();
    vector<ofVideoDevice> getDevices();

    //Getters and setters
    void setBackgroundSubtract(bool value);
    void setBlur(bool value);
    void setBlurAmount(float value);
    void setThreshold(float value);
    void setTolerance(float value);
    void setRemoveAfterSeconds(float value);
    void setEdgeThreshold(float value);
    void setMinBlobSize(float value);
    void setOutdoorMode(bool value);
    bool getBackgroundSubtract();
    bool getBlur();
    float getBlurAmount();
    float getThreshold();
    float getTolerance();
    float getRemoveAfterSeconds();
    float getEdgeThreshold();
    int getWidth();
    int getHeight();
    float getMinBlobSize();
    bool getOutdoorMode();
    int getWebcamIndex();
    int getNumActiveBlobs();
    vector<ofxWebcamBlob> getActiveBlobs();
    bool isOverlapCandidate(ofxWebcamBlob blob);
    bool thereAreOverlaps();
    bool shouldGrabBackground();
    ofxWebcamBlob * getOverlapBlob();

    //Action Methods
    void init(vector<ofVideoDevice> active, int resolutionWidth=DEFAULT_RES_WIDTH, int resolutionHeight=DEFAULT_RES_HEIGHT);
    void init(int resolutionWidth=DEFAULT_RES_WIDTH, int resolutionHeight=DEFAULT_RES_HEIGHT);
    void update();
    void grabBackground();
    void subtractBackground();

    //The Tracker
    void matchAndUpdateBlobs();
    void clearBlobs();

    //Image Getters
    ofxCvColorImage getColorImage();
    ofxCvGrayscaleImage getGrayImage();


    //Draw and debug methods
    void draw();
    void drawRGB(float x, float y);
    void drawRGB(float x, float y, float scale);
    void drawGrayscale(float x, float y);
    void drawGrayscale(float x, float y, float scale);
    void drawBlobPositions(float x, float y);
    void drawBlobPositions(float x, float y, float scale);
    void drawBackground(float x, float y);
    void drawBackground(float x, float y, float scale);
    void drawContours(float x, float y);
    void drawContours(float x, float y, float scale);
    void drawDiff(float x, float y);
    void drawDiff(float x, float y, float scale);
    void drawDebug(float x, float y);
    void drawDebug(float x, float y, float scale);
    void drawEdgeThreshold(float x, float y);
    void drawEdgeThreshold(float x, float y, float scale);

    //Calibration
    void calibratePosition(int index, ofPoint p);

    //closing
    void close();
};
