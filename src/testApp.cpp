#include "testApp.h"

std::string exec(char* cmd)
{
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe))
    {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

//--------------------------------------------------------------
void testApp::setup()
{
    hideGUI = false;
    kelvinCold = 6500;
    kelvinWarm = 4200;

    kelvinColdRange = kelvinCold;
    kelvinWarmRange = kelvinWarm;
    temperatureSpeed = 0.5;
    temperatureSpread = 0.5;
    temperatureTime = 0;

    brightnessRangeFrom = 0.0;
    brightnessRangeTo = 1.0;
    brightnessSpeed = 0.4;
    brightnessSpread = 0.25;
    brightnessTime = 0;

    setGUI();

    ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);

    // Setup the client, this connects to the server
    if (!ola_client.Setup())
    {
        std::cerr << "OLA Setup failed" << std::endl;
    }

//    sourceCamera.connect();
//	  cameraController.load("cameraSettings.xml");


    cam.setTranslationKey(' ');
    cam.enableOrtho();

    int tsqSize = 30;

    int tesselationHeight = 6;

    // construct tesselations

    ofVec3f origin;

    /** such is the mapping of dmx channels to the ofVec3f objects

             /     \
            |\  0  /|
            | \   / |
            |  \ /  |
            ­\ 1 | 2 /
             \  |  /

    **/


    ofVec3f * tesselation1map[] =
    {
        NULL,               NULL,               NULL,               NULL,               NULL,               NULL,
        new ofVec3f(0,0,53),new ofVec3f(),      NULL,               NULL,               NULL,               NULL,
        new ofVec3f(),      new ofVec3f(),      new ofVec3f(0,0,49),new ofVec3f(0,0,33),new ofVec3f(0,0,5), NULL,
        new ofVec3f(),      new ofVec3f(0,61,0),new ofVec3f(57,0,0),new ofVec3f(0,0,37),new ofVec3f(0,0,13),new ofVec3f(0,0,1),
        new ofVec3f(0,0,45),new ofVec3f(),      new ofVec3f(0,0,41),new ofVec3f(0,0,29),new ofVec3f(0,0,21),new ofVec3f(0,0,9),
        NULL,               new ofVec3f(),      new ofVec3f(),      NULL,               new ofVec3f(0,0,25),new ofVec3f(0,0,17)
    };
    int tesselation1width = 6;
    origin = addTesselation(origin, tsqSize, tesselation1width, tesselationHeight, tesselation1map);

    ofVec3f * tesselation2map[] =
    {
        NULL,               NULL,               NULL,               NULL,               NULL,               NULL,
        NULL,               new ofVec3f(),      new ofVec3f(0,0,69),NULL,               NULL,               NULL,
        new ofVec3f(0,0,93),new ofVec3f(),      new ofVec3f(),      new ofVec3f(),      new ofVec3f(0,0,65),NULL,
        new ofVec3f(),      new ofVec3f(0,89,0),new ofVec3f(81,0,0),new ofVec3f(0,0,77),new ofVec3f(),      new ofVec3f(),
        new ofVec3f(),      new ofVec3f(),      new ofVec3f(0,0,85),new ofVec3f(),      new ofVec3f(0,0,73),NULL,
        new ofVec3f(),      NULL,               new ofVec3f(),      new ofVec3f(),      new ofVec3f(),      NULL
    };
    int tesselation2width = 6;
    origin*=1.35;
    origin += addTesselation(origin, tsqSize, tesselation2width, tesselationHeight, tesselation2map);

    ofVec3f * tesselation3map[] =
    {
        //  1                   2                   3                   4                   5                   6                   7                   8                   9
        NULL,               NULL,               NULL,               new ofVec3f(),      NULL,               NULL,               NULL,               NULL,               NULL,
        NULL,               NULL,               new ofVec3f(),      new ofVec3f(),      new ofVec3f(),      NULL,               new ofVec3f(),      NULL,               NULL,
        new ofVec3f(0,0,121),new ofVec3f(),     new ofVec3f(0,0,109),new ofVec3f(0,113,0),new ofVec3f(),    new ofVec3f(),      new ofVec3f(),      new ofVec3f(),      new ofVec3f(),
        new ofVec3f(),      new ofVec3f(),      new ofVec3f(0,125,0),new ofVec3f(105,0,0),new ofVec3f(0,0,117),new ofVec3f(0,97,0),new ofVec3f(),   new ofVec3f(),      new ofVec3f(),
        new ofVec3f(),      new ofVec3f(),      new ofVec3f(),      new ofVec3f(),      new ofVec3f(),      new ofVec3f(101,0,0),new ofVec3f(),     new ofVec3f(),      new ofVec3f(),
        NULL,               NULL,               NULL,               new ofVec3f(),      new ofVec3f(),      new ofVec3f(),      NULL,               NULL,               NULL
    };

    int tesselation3width = 9;
    origin*=1.125;
    origin += addTesselation(origin, tsqSize, tesselation3width, tesselationHeight, tesselation3map);

    origin*=1.15;

    ofQuaternion rot = ofQuaternion(45, ofVec3f(0,0,1), 90, ofVec3f(1,0,0), 33, ofVec3f(1,0,0));
    ofMatrix4x4 mat;
    rot.get(mat);

    for(std::vector<TesselationSquare*>::iterator it = tesselation.begin(); it != tesselation.end(); ++it)
    {
        TesselationSquare* t = *(it);
        t->setGlobalPosition(t->getPosition()-(origin*.5));
        ofQuaternion rotQuat = t->getOrientationQuat();
        t->rotateAround(rot, ofVec3f(0,0,0));
        rotQuat *= rot;
        t->setOrientation(rotQuat);
        t->setGlobalPosition(t->getPosition()+ofVec3f(-3*tsqSize,3*tsqSize, 0));

    }

    // Directional Lights emit light based on their orientation, regardless of their position //
    directionalLight.setDiffuseColor(ofColor(255.f, 255.f, 255.f));
    directionalLight.setSpecularColor(ofColor(255.f, 255.f, 255.f));
    directionalLight.setDirectional();

    // set the direction of the light
    directionalLight.setOrientation( ofVec3f(-45, -45, 0) );
    ofSetSmoothLighting(true);
}

ofVec3f testApp::addTesselation(ofVec3f _origin, int _size, int _width, int _height, ofVec3f** _tesselationMap)
{

    ofVec3f offset, firstOffset;

    for (int i = 0; i < _height*_width ; i++)
    {
        int x = i%_width;
        int y = i/_width;

        ofColor c;

        offset = ofVec3f(-x*_size,(x+y)*_size,y*_size);

        if(y < 1) firstOffset = offset;

        ofVec3f* adresses = _tesselationMap[((_width-1)-x)+(_width*y)];

        if(adresses != NULL)
        {

            c = ofColor(255,255,255,255);

            TesselationSquare * tsq = new TesselationSquare();
            tsq->set(_size,_size);
            tsq->setGlobalPosition(ofVec3f(0, 0, -_size/2) + _origin + offset);
            tsq->setOrientation(ofVec3f(0,0,0));
            tsq->setup(floor(adresses->x));
            tsq->kelvinCold = kelvinCold;
            tsq->kelvinWarm = kelvinWarm;
            tsq->setColor(c);
            tesselation.push_back(tsq);

            tsq = new TesselationSquare();
            tsq->set(_size,_size);
            tsq->setGlobalPosition(ofVec3f(0,_size/2,0) + _origin + offset);
            tsq->setOrientation(ofVec3f(90,0,0));
            tsq->setup(floor(adresses->y));
            tsq->kelvinCold = kelvinCold;
            tsq->kelvinWarm = kelvinWarm;
            tsq->setColor(c);
            tesselation.push_back(tsq);

            tsq = new TesselationSquare();
            tsq->set(_size,_size);
            tsq->setGlobalPosition(ofVec3f(_size/2,0,0) + _origin + offset);
            tsq->setOrientation(ofVec3f(0,90,0));
            tsq->setup(floor(adresses->z));
            tsq->kelvinCold = kelvinCold;
            tsq->kelvinWarm = kelvinWarm;
            tsq->setColor(c);
            tesselation.push_back(tsq);

        }
    }
    return firstOffset;
}

void testApp::setGUI(){

	gui = new ofxUISuperCanvas("LEDlys Tesselation 0.9b");
//    gui->addSpacer();
//    gui->addLabel("Press 'h' to Hide GUIs", OFX_UI_FONT_SMALL);

    gui->setFont("GUI/DroidSans.ttf");
    gui->setFontSize(OFX_UI_FONT_LARGE, 10);
    gui->setFontSize(OFX_UI_FONT_MEDIUM, 8);
    gui->setFontSize(OFX_UI_FONT_SMALL, 6);
    gui->addSpacer();
	gui->addLabel("");
	gui->addLabel("Temperature", OFX_UI_FONT_LARGE);
    gui->addSpacer();
    gui->addLabel("Range", OFX_UI_FONT_SMALL);
	gui->addRangeSlider("tRange", kelvinWarm, kelvinCold, &kelvinWarmRange, &kelvinColdRange);
    gui->addSpacer();
    gui->addLabel("Speed", OFX_UI_FONT_SMALL);
    gui->addSlider("tSpeed",0,1,&temperatureSpeed);
    gui->addSpacer();
    gui->addLabel("Spread", OFX_UI_FONT_SMALL);
    gui->addSlider("tSpread",0,1,&temperatureSpread);
    gui->addSpacer();
	gui->addLabel("");
	gui->addLabel("Brightness", OFX_UI_FONT_LARGE);
    gui->addSpacer();
    gui->addLabel("Range", OFX_UI_FONT_SMALL);
	gui->addRangeSlider("bRange", 0, 1, &brightnessRangeFrom, &brightnessRangeTo);
    gui->addSpacer();
    gui->addLabel("Speed", OFX_UI_FONT_SMALL);
    gui->addSlider("bSpeed",0,1,&brightnessSpeed);
    gui->addSpacer();
    gui->addLabel("Spread", OFX_UI_FONT_SMALL);
    gui->addSlider("bSpread",0,1,&brightnessSpread);
    gui->addSpacer();
    gui->addLabel("");
    gui->addFPS(OFX_UI_FONT_SMALL);

    //TODO: Make GUI for brightness


    gui->autoSizeToFitWidgets();
    gui->getRect()->setHeight(ofGetHeight());


    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.getName();
	int kind = e.getKind();
//	cout << "got event from: " << name << endl;

}


//--------------------------------------------------------------
void testApp::update()
{

    if(ofGetFrameNum() > 1) {
            cam.disableMouseInput();
    }

    buffer.Blackout();

    float temperatureSpreadCubic = powf(temperatureSpread, 3);
    float brightnessSpreadCubic = powf(brightnessSpread, 3);

    for(std::vector<TesselationSquare*>::iterator it = tesselation.begin(); it != tesselation.end(); ++it)
    {
        TesselationSquare* t = *(it);
        float tempNoise = ofNoise(t->getX()*temperatureSpreadCubic, t->getY()*temperatureSpreadCubic, temperatureTime);
        unsigned int temp = round(ofMap(tempNoise, 0, 1, fmaxf(t->kelvinWarm, kelvinWarmRange), fminf(t->kelvinCold, kelvinColdRange)));
        t->setTemperature(temp);

        float brightness = ofNoise(t->getX()*brightnessSpreadCubic, t->getY()*brightnessSpreadCubic, brightnessTime);
        brightness = ofMap(brightness, 0, 1, brightnessRangeFrom, brightnessRangeTo);
        if(t->DMXstartAddress > 0)
        {
            t->setBrightness(brightness);
        }

        for(std::vector<DMXchannel*>::iterator chIt = t->DMXchannels.begin(); chIt != t->DMXchannels.end(); ++chIt)
        {
            DMXchannel* c = *(chIt);
            float value = 0;
            if(c->type == DMX_CHANNEL_CW)
            {
                value = ofMap(t->temperature, t->kelvinWarm, t->kelvinCold, 0, 1.);
                value = fminf(1.,ofMap(value, 0 , 0.5, 0., 1.));
            }
            if(c->type == DMX_CHANNEL_WW)
            {
                value = ofMap(t->temperature, t->kelvinCold, t->kelvinWarm, 0, 1.);
                value = fminf(1.,ofMap(value, 0 , 0.5, 0., 1.));
            }
            value *= t->brightness;

            if(c->width16bit)
            {
                unsigned int valueInt = ofMap(value, 0.,1., 0, pow(255,2));
                buffer.SetChannel(c->address-1, valueInt/255);
                buffer.SetChannel(c->address, valueInt%255);
            }
            else
            {
                ofLogError() << "8 bit not implemented" << std::endl;
            }
        }
    }

   if (!ola_client.SendDmx(0, buffer))
    {
        cout << "Send DMX failed" << endl;
    }

    //  cameraController.update();

    float now = ofGetElapsedTimef();
    temperatureTime += powf(temperatureSpeed,8) * ( ( now - lastFrameSeconds ) / (1./60));
    brightnessTime += powf(brightnessSpeed,8) * ( ( now - lastFrameSeconds ) / (1./60));
    lastFrameSeconds = now;
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackgroundGradient(ofColor(40), ofColor(10), OF_GRADIENT_CIRCULAR);
    glEnable(GL_DEPTH_TEST);
    ofViewport(gui->getRect()->getWidth(),0,ofGetWidth()-gui->getRect()->getWidth(),ofGetHeight());
    for(std::vector<TesselationSquare*>::iterator it = tesselation.begin(); it != tesselation.end(); ++it)
    {
        TesselationSquare* t = *(it);
        cam.begin();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        if(t->DMXstartAddress < 1)
        {
            ofEnableLighting();
            directionalLight.enable();
        }
        t->draw();
        if(t->DMXstartAddress < 1)
        {
            directionalLight.disable();
            ofDisableLighting();
        }
        cam.end();
    }
    ofViewport();

//    cameraController.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
    if(key == 'f') ofToggleFullscreen();
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{
    gui->getRect()->setHeight(ofGetHeight());
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}
