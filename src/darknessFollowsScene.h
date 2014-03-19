#pragma once
#include "ledScene.h"

class ChromaWhiteSpot : public ofLight, public LedFixture
{
public:

    void setup(unsigned int startAddress = 0)
    {
        DMXstartAddress = startAddress;
        if(startAddress > 0)
        {
            DMXchannels.push_back(new DMXchannel(startAddress, DMX_CHANNEL_COLOR_TEMPERATURE, false));
            DMXchannels.push_back(new DMXchannel(startAddress+1, DMX_CHANNEL_BRIGHTNESS, false));
        }
        ofLight::setSpotlight();
        ofLight::setSpecularColor(ofColor(0.0));
        ofLight::setSpotlightCutOff(33);
        ofLight::setSpotConcentration(25);
        ofLight::setAttenuation(1.f, .01f, 0.f);
        color = ofColor(255,255);
        kelvinCold = 6500;
        kelvinWarm = 2700;
        brightness = 1.0;
    };

    void update()
    {
        drawColor = color * brightness;
        drawColor *= temperatureToColor(temperature);
    };

    void draw()
    {
        ofLight::setDiffuseColor(drawColor);
        ofPushStyle();
        ofSetColor(drawColor);
        ofLight::draw();
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


class darknessFollowsScene : public ledScene
{
public:
    darknessFollowsScene();
    void setup();
    void setGUI(ofxUISuperCanvas* gui);
    void update(ola::DmxBuffer * buffer);
    void draw();

    vector<ChromaWhiteSpot*> spotlights;

    ofPlanePrimitive floor;
    ofMaterial floorMaterial;

    ofEasyCam cam;
    ofShader lightShader;

protected:
private:
};
