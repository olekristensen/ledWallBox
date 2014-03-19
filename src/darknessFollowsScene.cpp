#include "darknessFollowsScene.h"

darknessFollowsScene::darknessFollowsScene()
{
    //ctor
}

void darknessFollowsScene::setup()
{
    name = "Darkness Follows";

    int numberSpotlightsX = 3;
    int numberSpotlightsY = 3;
    float spotlightsDistX = 110;
    float spotlightsDistY = 85;
    float spotlightsPosZ = -285;

    floor.set(285,300,100,100);
    floorMaterial.setColors(ofColor(200,200,200), ofColor(0,0,0), ofColor(0,0,0), ofColor(0,0,0));

    int startAddress = 1;

    ofVec3f posOffsetFromCenter(spotlightsDistX*(numberSpotlightsX-1)*.5, spotlightsDistY*(numberSpotlightsY-1)*.5, spotlightsPosZ);

    for(int x = 0; x < numberSpotlightsX; x++)
    {
        for(int y = 0; y < numberSpotlightsY; y++)
        {
            ChromaWhiteSpot* cws = new ChromaWhiteSpot();
            cws->setup(startAddress);
            startAddress+=2;
            cws->setParent(floor);
            ofVec3f pos(x*spotlightsDistX, y*spotlightsDistY);
            pos -= posOffsetFromCenter;
            cws->setPosition(pos);
            spotlights.push_back(cws);
        }
    }

    cam.setTranslationKey(' ');
    lightShader.load("shaders/darknessFollowsFloor");

}

void darknessFollowsScene::setGUI(ofxUISuperCanvas* gui)
{
    this->gui = gui;

}

void darknessFollowsScene::update(ola::DmxBuffer * buffer)
{
    for(vector<ChromaWhiteSpot*>::iterator it = spotlights.begin(); it != spotlights.end(); it++){
        ChromaWhiteSpot * cws = *(it);
        float temperature = ofMap(ofNoise(ofGetElapsedTimef(), cws->getPosition().x*0.01, cws->getPosition().y*0.01),0,1,cws->kelvinWarm,cws->kelvinCold);
        float brightness = ofNoise(ofGetElapsedTimef()+200, cws->getPosition().x*0.1, cws->getPosition().y*0.1);

        cws->setTemperature(temperature);
        cws->setBrightness(brightness);

            for(std::vector<DMXchannel*>::iterator chIt = cws->DMXchannels.begin(); chIt != cws->DMXchannels.end(); chIt++)
            {
                DMXchannel* c = *(chIt);
                float value = 0;
                if(c->type == DMX_CHANNEL_BRIGHTNESS)
                {
                    value = cws->brightness;
                }
                if(c->type == DMX_CHANNEL_COLOR_TEMPERATURE)
                {
                    value = ofMap(cws->temperature, cws->kelvinWarm, cws->kelvinCold, 0, 1.);
                }

                if(c->width16bit)
                {
                    unsigned int valueInt = ofMap(value, 0.,1., 0, pow(255,2));
                    buffer->SetChannel(c->address-1, valueInt/255);
                    buffer->SetChannel(c->address, valueInt%255);
                }
                else
                {
                    unsigned int valueInt = ofMap(value, 0.,1., 0, 255);
                    buffer->SetChannel(c->address-1, valueInt);
                }
            }
        cws->update();

    }

}

void darknessFollowsScene::draw()
{
//    ofPushMatrix();
//    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    cam.begin();

    ofPushStyle();
    ofEnableLighting();
    for(vector<ChromaWhiteSpot*>::iterator it = spotlights.begin(); it != spotlights.end(); ++it){
        ChromaWhiteSpot * cws = *(it);
        cws->enable();
    }
    floorMaterial.begin();
    //lightShader.begin();
    floor.draw();
    //lightShader.end();
    floorMaterial.end();
    for(vector<ChromaWhiteSpot*>::iterator it = spotlights.begin(); it != spotlights.end(); ++it){
        ChromaWhiteSpot * cws = *(it);
        cws->draw();
        cws->disable();
    }

    ofDisableLighting();
    ofPopStyle();
    ofDrawAxis(200);
    cam.end();
//    ofPopMatrix();
}
