#pragma once

#include "ofMain.h"
#include "pgCamera.h"
#include "CameraController.h"

enum DMXchannelType
{
    DMX_CHANNEL_RED,
    DMX_CHANNEL_GREEN,
    DMX_CHANNEL_BLUE,
    DMX_CHANNEL_WHITE,
    DMX_CHANNEL_CW,
    DMX_CHANNEL_WW,
    DMX_CHANNEL_COLOR_TEMPERATURE,
    DMX_CHANNEL_BRIGHTNESS,
    DMX_CHANNEL_HUE,
    DMX_CHANNEL_SATURATIOM
};

class DMXchannel
{

public:
    DMXchannel(unsigned int address, DMXchannelType type = DMX_CHANNEL_BRIGHTNESS, bool width16bit = false, unsigned int minValue = 0, unsigned int maxValue=255)
    {
        this->address = address;
        this->type = type;
        this->width16bit = width16bit;
        this->minValue = minValue;
        this->maxValue = maxValue;
    };

    DMXchannelType type;
    unsigned int address;
    unsigned int minValue;
    unsigned int maxValue;
    bool width16bit;

};

class LedFixture
{

public:
    vector <DMXchannel*> DMXchannels;
    int DMXstartAddress;

    virtual void setColor(ofColor c) = 0;
    virtual ofColor getColor() = 0;

    virtual void setTemperature(unsigned int degreesKelvin) = 0;
    virtual unsigned int getTemperature() = 0;

    virtual void setBrightness(float brightness) = 0;
    virtual float getBrightness() = 0;

    void setup();
    void update();
    void draw();

    unsigned int kelvinCold;
    unsigned int kelvinWarm;

};

class TesselationSquare : public ofPlanePrimitive, public LedFixture
{
public:

    ofColor color;
    void setup(unsigned int startAddress = 0)
    {
        DMXstartAddress = startAddress;
        if(startAddress > 0){
            DMXchannels.push_back(new DMXchannel(startAddress, DMX_CHANNEL_WW, true));
            DMXchannels.push_back(new DMXchannel(startAddress+2, DMX_CHANNEL_CW, true));
        }
    };

    void update()
    {
        ;
    };

    void draw()
    {
        ofPushStyle();
        ofSetColor(color);
        ofPlanePrimitive::draw();
        ofPopStyle();
    };

    void setColor(ofColor color)
    {
        this->color = color;
    }

    ofColor getColor()
    {
        return color;
    }

    void setTemperature(unsigned int degreesKelvin)
    {
        ofLog(OF_LOG_WARNING, "Not implemented");
    }

    unsigned int getTemperature()
    {
        ofLog(OF_LOG_WARNING, "Not implemented");
        return 0;
    }

    void setBrightness(float brightness)
    {
        ofLog(OF_LOG_WARNING, "Not implemented");
    }

    float getBrightness()
    {
        ofLog(OF_LOG_WARNING, "Not implemented");
        return 1.0;
    }

};


class testApp : public ofBaseApp
{

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
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    ofVec3f addTesselation(ofVec3f _origin, int _size, int _width, int _height, int* _tesselationMap);

protected:
    //pgCamera sourceCamera;
    //pgCamera destinationCamera;

    CameraController cameraController;
    vector<TesselationSquare*> tesselation;
    ofEasyCam cam;
    ofLight directionalLight;
};
