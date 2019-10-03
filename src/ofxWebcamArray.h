#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"

#define DEFAULT_RES_WIDTH 640
#define DEFAULT_RES_HEIGHT 360

class ofxWebcamImageCalibration
{
  private:
    int width;
    int height;
    ofVec2f position;
    ofVec2f scale;
    float rotation;

  public:
    ofxWebcamImageCalibration() : width(DEFAULT_RES_WIDTH), height(DEFAULT_RES_HEIGHT), position(0.0f, 0.0f), scale(1.0f, 1.0f), rotation(0) {
    }

    ofxWebcamImageCalibration(int index, int resolutionWidth=DEFAULT_RES_WIDTH, int resolutionHeight=DEFAULT_RES_HEIGHT) : scale(1.0f, 1.0f), rotation(0) {
      width = resolutionWidth;
      height = resolutionHeight;
      position.x = width * index;
      position.y = 0.0f;
      ofLogNotice("ofxWebcamImageCalibration") << "Calibrating Webcam " << index << " to position: (" << position.x << ", " << position.y << ")";
    }

    ~ofxWebcamImageCalibration(){

    }

    void setPosition(float x, float y)
    {
      position.x = x;
      position.y = y;
    }

    void setPosition(ofVec2f vec)
    {
      position = vec;
    }

    void setScale(float x, float y)
    {
      scale.x = x;
      scale.y = y;
    }

    void setScale(ofVec2f vec)
    {
      scale = vec;
    }

    void setRotation(float angle)
    {
      rotation = angle;
    }

    void setDimensions(int w, int h)
    {
      width = w;
      height = h;
    }

    ofVec2f getPosition(){
      return position;
    }

    ofVec2f getScale(){
      return scale;
    }

    float getRotation()
    {
      return rotation;
    }

    ofRectangle getBoundingRect()
    {
      if(rotation != 0)
      {
        float p1x, p1y, p2x, p2y, p3x, p3y, p4x, p4y, d, alpha, beta;

        alpha = ofDegToRad(rotation);
        d = sqrt((width*scale.x)*(width*scale.x) + (height*scale.y)*(height*scale.y));
        beta = acos((width*scale.x)/d);

        p1x = position.x;
        p1y = position.y;
        p2x = cos(alpha)*(width*scale.x) + p1x;
        p2y = sin(alpha)*(width*scale.x) + p1y;
        p3x = cos(alpha+beta)*d + p1x;
        p3y = sin(alpha+beta)*d + p1y;
        p4x = cos(alpha+HALF_PI)*(height*scale.y) + p1x;
        p4y = sin(alpha+HALF_PI)*(height*scale.y) + p1y;

        std::vector<float> xs = {p1x, p2x, p3x, p4x};
        std::vector<float> ys = {p1y, p2y, p3y, p4y};

        float minx = *std::min_element(xs.begin(), xs.end());
        float miny = *std::min_element(ys.begin(), ys.end());
        float maxx = *std::max_element(xs.begin(), xs.end());
        float maxy = *std::max_element(ys.begin(), ys.end());

        ofRectangle rect(minx, miny, maxx-minx, maxy-miny);

        return rect;
      }
      else{
        return ofRectangle(position.x, position.y, (width*scale.x), (height*scale.y));
      }
    }
};

class ofxWebcamArray
{
  private:
    std::vector<ofVideoGrabber *> webcams;
    std::vector<ofxWebcamImageCalibration *> calibrations;
    vector<ofVideoDevice> devices;
    vector<ofVideoDevice> activeDevices;
    int detectedWebcamsCache;
    ofFbo colorFbo;
    ofPixels colorPixels;

  public:
    int width;
    int height;

    ofxWebcamArray() : detectedWebcamsCache(-1), width(0), height(0) {

    }

    ~ofxWebcamArray(){

    }

    vector<ofVideoDevice> getDevices(){
      if(detectedWebcamsCache == -1)
      {
        numWebcamsDetected();
      }

      return devices;

    }

    void setActiveDevices(vector<ofVideoDevice> devs) {
      activeDevices = devs;
    }

    vector<ofVideoDevice> getActiveDevices(){
      return activeDevices;
    }

    bool isActive(ofVideoDevice vd) {
      bool found = false;
      for (auto & elem : activeDevices)
      {
      	if (elem.id == vd.id)
      	{
      		found = true;
      		break;
      	}
      }
      if(found) {
      	return true;
      }
      else {
      	return false;
      }
    }

    int numWebcamsDetected()
    {
      if(detectedWebcamsCache == -1)
      {
        ofVideoGrabber vg;
        devices = vg.listDevices();
        detectedWebcamsCache = devices.size();
        return detectedWebcamsCache;
      }
      else{
        return detectedWebcamsCache;
      }
    }

    void init(vector<ofVideoDevice> active, int resolutionWidth=DEFAULT_RES_WIDTH, int resolutionHeight=DEFAULT_RES_HEIGHT)
    {
      if(numWebcamsDetected() > 0) {
        if(active.size() == 0){
          setActiveDevices(devices);
        }
        else {
          setActiveDevices(active);
        }

        ofLogNotice("ofxWebcamArray::init") << "Initializing " << activeDevices.size() << " Webcams.";
        for(uint8_t i=0; i<activeDevices.size(); i++)
        {
          ofVideoGrabber * v = new ofVideoGrabber();
          v->setDeviceID(activeDevices[i].id);
          v->setup(resolutionWidth,resolutionHeight);
          webcams.push_back(v);
          ofxWebcamImageCalibration * c = new ofxWebcamImageCalibration(i, resolutionWidth, resolutionHeight);
          calibrations.push_back(c);
          width += resolutionWidth;
          height = resolutionHeight;
        }

        ofLogNotice("ofWebcamArray") << "Alocating FBO of size: " << width << ", " << height;
        allocateImages();
      }
    }

    void init(int resolutionWidth=DEFAULT_RES_WIDTH, int resolutionHeight=DEFAULT_RES_HEIGHT){
      vector<ofVideoDevice> empty;
      init(empty, resolutionWidth, resolutionHeight);
    }

    void allocateImages()
    {
      colorFbo.allocate(width, height, GL_RGB);
      colorPixels.allocate(width, height, GL_RGB);
    }

    void update()
    {
      for(uint8_t i=0; i<webcams.size(); i++)
      {
        webcams[i]->update();
      }
    }

    void close()
    {
      for(uint8_t i=0; i<webcams.size(); i++)
      {
        webcams[i]->close();
      }
    }

    ofPixels & getPixels() //TODO: This is failing only a pixel on top!!
    {
      colorFbo.begin();
      glViewport(0, 0, width, height);
      ofClear(0, 0, 0);
      ofSetColor(255);
      ofEnableBlendMode(OF_BLENDMODE_SCREEN);
      for(uint8_t i=0; i<webcams.size(); i++)
      {
        ofPushMatrix();
        ofTranslate(calibrations[i]->getPosition().x, calibrations[i]->getPosition().y);
        ofRotateDeg(calibrations[i]->getRotation());
        ofScale(calibrations[i]->getScale().x, calibrations[i]->getScale().y);
        webcams[i]->draw(0,0);
        ofPopMatrix();
      }
      ofEnableBlendMode(OF_BLENDMODE_ALPHA);
      colorFbo.end();

      colorFbo.readToPixels(colorPixels);

      return colorPixels;
    }

    void calibratePosition(uint8_t index, ofPoint p)
    {
      if(index < calibrations.size())
      {
        calibrations[index]->setPosition(p);
      }
      else
      {
        ofLogError("ofxWebcamArray::calibratePosition") << "There is no Webcam with index " << index;
      }
    }
};
