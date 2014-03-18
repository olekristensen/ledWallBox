#pragma once
#include "ofMain.h"
#include "LedFixture.h"
#include <ola/DmxBuffer.h>
#include <ola/Logging.h>
#include <ola/StreamingClient.h>
#include "ofxUI.h"

class ledScene
{
    public:
        ledScene();
        ~ledScene();
        virtual void setup(){};
        virtual void draw(){};
        virtual void update(ola::DmxBuffer * buffer){};
        virtual void setGUI(ofxUISuperCanvas* gui){this->gui = gui;};
        void hideGUI();
        void showGUI();
        string name;
        vector<ofxUIWidget*> guiWidgets;
        ofxUISuperCanvas* gui;
    protected:
    private:
};
