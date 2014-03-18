#pragma once
#include "ledScene.h"

class darknessFollowsScene : public ledScene
{
public:
    darknessFollowsScene();
    void setup();
    void setGUI(ofxUISuperCanvas* gui);
    void update(ola::DmxBuffer * buffer);
    void draw();

protected:
private:
};
