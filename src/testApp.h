#pragma once

#include "ofMain.h"
#include "pgCamera.h"
#include "CameraController.h"
#include <ola/DmxBuffer.h>
#include <ola/Logging.h>
#include <ola/StreamingClient.h>
#include "fullScreenDialogs.h"
#include "ofxUI.h"

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
    DMX_CHANNEL_SATURATION
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

    static ofColor temperatureToColor(unsigned int temp)
    {

        float blackbodyColor[91*3] =
{
    1.0000, 0.0425, 0.0000, // 1000K
    1.0000, 0.0668, 0.0000, // 1100K
    1.0000, 0.0911, 0.0000, // 1200K
    1.0000, 0.1149, 0.0000, // ...
    1.0000, 0.1380, 0.0000,
    1.0000, 0.1604, 0.0000,
    1.0000, 0.1819, 0.0000,
    1.0000, 0.2024, 0.0000,
    1.0000, 0.2220, 0.0000,
    1.0000, 0.2406, 0.0000,
    1.0000, 0.2630, 0.0062,
    1.0000, 0.2868, 0.0155,
    1.0000, 0.3102, 0.0261,
    1.0000, 0.3334, 0.0379,
    1.0000, 0.3562, 0.0508,
    1.0000, 0.3787, 0.0650,
    1.0000, 0.4008, 0.0802,
    1.0000, 0.4227, 0.0964,
    1.0000, 0.4442, 0.1136,
    1.0000, 0.4652, 0.1316,
    1.0000, 0.4859, 0.1505,
    1.0000, 0.5062, 0.1702,
    1.0000, 0.5262, 0.1907,
    1.0000, 0.5458, 0.2118,
    1.0000, 0.5650, 0.2335,
    1.0000, 0.5839, 0.2558,
    1.0000, 0.6023, 0.2786,
    1.0000, 0.6204, 0.3018,
    1.0000, 0.6382, 0.3255,
    1.0000, 0.6557, 0.3495,
    1.0000, 0.6727, 0.3739,
    1.0000, 0.6894, 0.3986,
    1.0000, 0.7058, 0.4234,
    1.0000, 0.7218, 0.4485,
    1.0000, 0.7375, 0.4738,
    1.0000, 0.7529, 0.4992,
    1.0000, 0.7679, 0.5247,
    1.0000, 0.7826, 0.5503,
    1.0000, 0.7970, 0.5760,
    1.0000, 0.8111, 0.6016,
    1.0000, 0.8250, 0.6272,
    1.0000, 0.8384, 0.6529,
    1.0000, 0.8517, 0.6785,
    1.0000, 0.8647, 0.7040,
    1.0000, 0.8773, 0.7294,
    1.0000, 0.8897, 0.7548,
    1.0000, 0.9019, 0.7801,
    1.0000, 0.9137, 0.8051,
    1.0000, 0.9254, 0.8301,
    1.0000, 0.9367, 0.8550,
    1.0000, 0.9478, 0.8795,
    1.0000, 0.9587, 0.9040,
    1.0000, 0.9694, 0.9283,
    1.0000, 0.9798, 0.9524,
    1.0000, 0.9900, 0.9763,
    1.0000, 1.0000, 1.0000, /* 6500K */
    0.9771, 0.9867, 1.0000,
    0.9554, 0.9740, 1.0000,
    0.9349, 0.9618, 1.0000,
    0.9154, 0.9500, 1.0000,
    0.8968, 0.9389, 1.0000,
    0.8792, 0.9282, 1.0000,
    0.8624, 0.9179, 1.0000,
    0.8465, 0.9080, 1.0000,
    0.8313, 0.8986, 1.0000,
    0.8167, 0.8895, 1.0000,
    0.8029, 0.8808, 1.0000,
    0.7896, 0.8724, 1.0000,
    0.7769, 0.8643, 1.0000,
    0.7648, 0.8565, 1.0000,
    0.7532, 0.8490, 1.0000,
    0.7420, 0.8418, 1.0000,
    0.7314, 0.8348, 1.0000,
    0.7212, 0.8281, 1.0000,
    0.7113, 0.8216, 1.0000,
    0.7018, 0.8153, 1.0000,
    0.6927, 0.8092, 1.0000,
    0.6839, 0.8032, 1.0000,
    0.6755, 0.7975, 1.0000,
    0.6674, 0.7921, 1.0000,
    0.6595, 0.7867, 1.0000,
    0.6520, 0.7816, 1.0000,
    0.6447, 0.7765, 1.0000,
    0.6376, 0.7717, 1.0000,
    0.6308, 0.7670, 1.0000,
    0.6242, 0.7623, 1.0000,
    0.6179, 0.7579, 1.0000,
    0.6117, 0.7536, 1.0000,
    0.6058, 0.7493, 1.0000,
    0.6000, 0.7453, 1.0000,
    0.5944, 0.7414, 1.0000 /* 10000K */
};

        float alpha = (temp % 100) / 100.0;
        int temp_index = ((temp - 1000) / 100)*3;

        ofColor fromColor = ofColor(blackbodyColor[temp_index]*255, blackbodyColor[temp_index+1]*255, blackbodyColor[temp_index+2]*255);
        ofColor toColor = ofColor(blackbodyColor[temp_index+3]*255, blackbodyColor[temp_index+3+1]*255, blackbodyColor[temp_index+3+2]*255);

        return fromColor.lerp(toColor, alpha);
    };



};

class TesselationSquare : public ofPlanePrimitive, public LedFixture
{
public:

    void setup(unsigned int startAddress = 0)
    {
        DMXstartAddress = startAddress;
        if(startAddress > 0)
        {
            DMXchannels.push_back(new DMXchannel(startAddress, DMX_CHANNEL_WW, true));
            DMXchannels.push_back(new DMXchannel(startAddress+2, DMX_CHANNEL_CW, true));
        }
        kelvinCold = 6500;
        kelvinWarm = 4200;
        brightness = 1.0;
    };

    void update()
    {
        drawColor = color * brightness;
        if(DMXstartAddress > 0)
        {
            drawColor *= temperatureToColor(temperature);
        }
        else
        {
            drawColor *= temperatureToColor(round(ofMap(0.25, 0,1, kelvinCold, kelvinWarm)));
            drawColor *= 0.75;
        }
    };

    void draw()
    {
        //ofPushStyle();
        ofSetColor(drawColor);
        ofPlanePrimitive::draw();
        //ofPopStyle();
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
        temperature = degreesKelvin;
    }

    unsigned int getTemperature()
    {
        return temperature;
    }

    void setBrightness(float brightness)
    {
        this->brightness = brightness;
    }

    float getBrightness()
    {
        return brightness;
    }

    float brightness;
    unsigned int temperature;

    ofColor color;
    ofColor drawColor;

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

    void setGUI();

    ofVec3f addTesselation(ofVec3f _origin, int _size, int _width, int _height, ofVec3f** _tesselationMap);

protected:
    //pgCamera sourceCamera;
    //pgCamera destinationCamera;

    CameraController cameraController;
    vector<TesselationSquare*> tesselation;
    ofEasyCam cam;
    ola::DmxBuffer buffer;
    ola::StreamingClient ola_client;

    ofImage perlinNoiseImage;
    ofRectangle tesselationRect;

    float lastFrameSeconds;

    unsigned int kelvinCold;
    unsigned int kelvinWarm;

    float kelvinWarmRange;
    float kelvinColdRange;
    float temperatureSpeed;
    float temperatureTime;
    float temperatureSpread;

    float brightnessRangeFrom;
    float brightnessRangeTo;
    float brightnessSpeed;
    float brightnessTime;
    float brightnessSpread;

    float timeOffset = 100.0;

    ofxUISuperCanvas *gui;
    bool hideGUI;
    bool bSaveSettings;
    bool bLoadSettings;
    void guiEvent(ofxUIEventArgs &e);
};
