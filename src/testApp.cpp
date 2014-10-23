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

    printFontHeader.loadFont("GUI/DroidSans.ttf", 12, true, true, true, 0);
    printFontText.loadFont("GUI/DroidSans.ttf", 8, true, true, true, 0);

    loadedFileName = "unsaved preset";

    hideGUI = false;
    kelvinCold = 6500;
    kelvinWarm = 4200;

    kelvinColdRange = kelvinCold;
    kelvinWarmRange = kelvinWarm;
    temperatureSpeed = 0.5;
    temperatureSpread = 0.15;
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

    cam.setTranslationKey(' ');
    cam.enableOrtho();

    int tsqSize = 90;

    int wallWidth = 8;
    int wallHeight = 3;

    // construct tesselations

    ofVec3f origin;

/* new ofVec3f(141,129,133),
    new ofVec3f(137,145,149),
    new ofVec3f(165,153,157),
    new ofVec3f(161,169,173),
    new ofVec3f(189,177,181),
    new ofVec3f(185,193,197),
    new ofVec3f(213,201,205),
    new ofVec3f(209,217,221)
*/
    int wallMap[8][3] =
    {
        {141,129,133 },
        {137,145,149 },
        {165,153,157 },
        {161,169,173 },
        {189,177,181 },
        {185,193,197 },
        {213,201,205 },
        {209,217,221 }
    };

    for(int x = 0; x < wallWidth; x++){
        for(int y = 0; y < wallHeight; y++){

        int address = wallMap[x][y];

        ofVec2f position(x*tsqSize, y*tsqSize);
        position -= ofVec2f(tsqSize*wallWidth/2.0, tsqSize*wallHeight/2.0);

        ofColor c = ofColor(255,255,255,255);

        TesselationSquare * tsq = new TesselationSquare();
        tsq->set(tsqSize,tsqSize, 2, 2);
        tsq->setGlobalPosition(ofVec3f(position.x, position.y, 0));
        tsq->setOrientation(ofVec3f(0,0,0));
        tsq->setup(floor(address));
        tsq->kelvinCold = kelvinCold;
        tsq->kelvinWarm = kelvinWarm;
        tsq->setColor((address > 0)? c : ofColor(255,255) );
        tesselation.push_back(tsq);

        }
    }

/*    ofQuaternion rot = ofQuaternion(45, ofVec3f(0,0,1), 90, ofVec3f(1,0,0), 33, ofVec3f(1,0,0));
    ofMatrix4x4 mat;
    rot.get(mat);
*/
    for(std::vector<TesselationSquare*>::iterator it = tesselation.begin(); it != tesselation.end(); ++it)
    {
        TesselationSquare* t = *(it);
        //t->setGlobalPosition(t->getPosition()-(origin*.5));
        //ofQuaternion rotQuat = t->getOrientationQuat();
        //t->rotateAround(rot, ofVec3f(0,0,0));
        //rotQuat *= rot;
        //t->setOrientation(rotQuat);
//        t->setGlobalPosition(t->getPosition()+ofVec3f(0,3*tsqSize, 0));
        tesselationRect.growToInclude(t->getPosition()+ofVec3f(tsqSize/2.0, tsqSize/2.0));
        tesselationRect.growToInclude(t->getPosition()-ofVec3f(tsqSize/2.0, tsqSize/2.0));
    }

    perlinNoiseImage.allocate(int(tesselationRect.getWidth()*0.25), int(tesselationRect.getHeight()*0.25),  OF_IMAGE_COLOR);

    ofSetFrameRate(30);
}

void testApp::setGUI()
{

    gui = new ofxUISuperCanvas("LEDlys WallBox 0.9b");
//    gui->addLabel("Press 'h' to Hide GUIs", OFX_UI_FONT_SMALL);
    gui->setWidth(ofGetWidth()/3.);
    gui->setFont("GUI/DroidSans.ttf");
    gui->setFontSize(OFX_UI_FONT_LARGE, 10);
    gui->setFontSize(OFX_UI_FONT_MEDIUM, 8);
    gui->setFontSize(OFX_UI_FONT_SMALL, 6);
    gui->addLabel("");
    gui->addLabel("Temperature", OFX_UI_FONT_LARGE);
    gui->addSpacer();
    gui->addLabel("Range", OFX_UI_FONT_SMALL);
    gui->addRangeSlider("tRange", kelvinWarm, kelvinCold, &kelvinWarmRange, &kelvinColdRange, gui->getRect()->getWidth()-8, 30)->setColorBack(ofColor(48,48,48));
    gui->addSpacer();
    gui->addLabel("Speed", OFX_UI_FONT_SMALL);
    gui->addSlider("tSpeed",0,1,&temperatureSpeed, gui->getRect()->getWidth()-8, 30)->setColorBack(ofColor(48,48,48));
    gui->addSpacer();
    gui->addLabel("Spread", OFX_UI_FONT_SMALL);
    gui->addSlider("tSpread",0,0.33,&temperatureSpread, gui->getRect()->getWidth()-8, 30)->setColorBack(ofColor(48,48,48));
    gui->addSpacer();
    gui->addLabel("");
    gui->addLabel("Brightness", OFX_UI_FONT_LARGE);
    gui->addSpacer();
    gui->addLabel("Range", OFX_UI_FONT_SMALL);
    gui->addRangeSlider("bRange", 0, 1, &brightnessRangeFrom, &brightnessRangeTo, gui->getRect()->getWidth()-8, 30)->setColorBack(ofColor(48,48,48));
    gui->addSpacer();
    gui->addLabel("Speed", OFX_UI_FONT_SMALL);
    gui->addSlider("bSpeed",0,1,&brightnessSpeed, gui->getRect()->getWidth()-8, 30)->setColorBack(ofColor(48,48,48));
    gui->addSpacer();
    gui->addLabel("Spread", OFX_UI_FONT_SMALL);
    gui->addSlider("bSpread",0,0.33,&brightnessSpread, gui->getRect()->getWidth()-8, 30)->setColorBack(ofColor(48,48,48));
    gui->addSpacer();
    gui->addLabel("");
    gui->addLabel("Presets", OFX_UI_FONT_LARGE);
    gui->addSpacer();
    gui->addLabel("Keep settings as xml files", OFX_UI_FONT_SMALL);
    gui->addSpacer();
    gui->addLabelButton("Load", bLoadSettings, gui->getRect()->getWidth()-8, 30)->setColorBack(ofColor(48,48,48));
    gui->addLabelButton("Save", bSaveSettings, gui->getRect()->getWidth()-8, 30)->setColorBack(ofColor(48,48,48));
    gui->addLabelButton("Save PDFs", bSavePDF, gui->getRect()->getWidth()-8, 30)->setColorBack(ofColor(48,48,48));
    gui->addSpacer();
    gui->addFPS();

    gui->getRect()->setHeight(ofGetHeight());
    gui->autoSizeToFitWidgets();


    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
    int kind = e.getKind();
    //cout << "got event from: " << name << endl;
    if(name == "Save")
    {
        ofxUIButton *button = (ofxUIButton *) e.getButton();
        if(button->getValue())
        {
            ofFileDialogResult dr = fullScreenSaveDialog( ofToDataPath("settings")+"/settings.xml", "Save settings");
            if(dr.bSuccess)
            {
                ofFile file(dr.filePath);
                if(file.getExtension() != "xml")
                {
                    file = ofFile(dr.filePath + ".xml");
                }
                gui->saveSettings(file.getAbsolutePath());
            }
        }
    }
    else if(name == "Load")
    {
        ofxUIButton *button = (ofxUIButton *) e.getButton();
        if(button->getValue())
        {

            ofFileDialogResult dr = fullScreenLoadDialog("Load settings", false, ofToDataPath("settings")+"/*.xml");

            if(dr.bSuccess)
            {
                ofFile file(dr.filePath);
                if(file.getExtension() == "xml")
                {
                    gui->loadSettings(file.getAbsolutePath());
                    loadedFileName = file.getBaseName();
                }
            }

        }
    }
    else if(name == "Save PDFs")
    {
        ofxUIButton *button = (ofxUIButton *) e.getButton();
        if(button->getValue())
        {


            string path = ofToDataPath("settings");
            ofDirectory dir(path);
            //only show png files
            dir.allowExt("xml");
            //populate the directory object
            dir.listDir();

            //go through and print out all the paths
            for(int i = 0; i < dir.numFiles(); i++)
            {
                ofLogNotice(dir.getPath(i));
                ofFile file(dir.getPath(i));
                gui->loadSettings(file.getAbsolutePath());
                loadedFileName = file.getBaseName();
                bSavePDF = true;
                update();
                draw();
            }
        }
    }



}


//--------------------------------------------------------------
void testApp::update()
{

    if(ofGetFrameNum() > 1)
    {
        cam.disableMouseInput();
    }

    buffer.Blackout();

    double temperatureSpreadCubic = powf(temperatureSpread, 3);
    double brightnessSpreadCubic = powf(brightnessSpread, 3);

    int imageWidth = perlinNoiseImage.getWidth();
    int imageHeight = perlinNoiseImage.getHeight();

    float zoomFactor = 0.33;

    for(int x = 0; x < imageWidth; x++)
    {
        for(int y = 0; y < imageHeight; y++)
        {
            double xMapped = ofMap(x, 0, imageWidth, tesselationRect.getMinX(), tesselationRect.getMaxX());
            double yMapped = ofMap(y, imageHeight, 0, tesselationRect.getMinY(), tesselationRect.getMaxY());

            float brightness = ofNoise(xMapped*brightnessSpreadCubic*zoomFactor, yMapped*brightnessSpreadCubic*zoomFactor, brightnessTime);
            brightness = ofMap(brightness, 0, 1, brightnessRangeFrom, brightnessRangeTo);

            float tempNoise = ofNoise(xMapped*temperatureSpreadCubic*zoomFactor, yMapped*temperatureSpreadCubic*zoomFactor, temperatureTime);
            unsigned int temp = round(ofMap(tempNoise, 0, 1, kelvinWarmRange, kelvinColdRange));

            ofColor c = LedFixture::temperatureToColor(temp);
            c *= brightness;

            perlinNoiseImage.setColor(x,y,c);
        }
    }
    perlinNoiseImage.update();

    #pragma omp parallel for
    for(int i = 0; i < tesselation.size(); i++)
    {
        TesselationSquare* t = tesselation[i];
        if(t->DMXchannels.size() > 0)
        {
            float tempNoise = ofNoise(t->getX()*temperatureSpreadCubic*zoomFactor, t->getY()*temperatureSpreadCubic*zoomFactor, temperatureTime);
            unsigned int temp = round(ofMap(tempNoise, 0, 1, fmaxf(t->kelvinWarm, kelvinWarmRange), fminf(t->kelvinCold, kelvinColdRange)));
            t->setTemperature(temp);

            float brightness = ofNoise(t->getX()*brightnessSpreadCubic*zoomFactor, t->getY()*brightnessSpreadCubic*zoomFactor, brightnessTime);
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
                    value = ofMap(t->temperature, t->kelvinCold, t->kelvinWarm, 0, 1.);
                    value = 1.0-powf(value-1.0, 4);
                    value *= .95;
                    //value = fminf(1.,ofMap(value, 0 , 0.5, 0., 1.));
                }
                if(c->type == DMX_CHANNEL_WW)
                {
                    value = ofMap(t->temperature, t->kelvinWarm, t->kelvinCold, 0, 1.);
                    value = 1.0-powf(value-1.0, 4);
                    //value = fminf(1.,ofMap(value, 0 , 0.5, 0., 1.));
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
        t->update();
    }

    if (!ola_client.SendDmx(0, buffer))
    {
        cout << "Send DMX failed" << endl;
    }

    //  cameraController.update();

    float now = ofGetElapsedTimef() + timeOffset;
    temperatureTime += powf(temperatureSpeed,8) * ( ( now - lastFrameSeconds ) / (1./60));
    brightnessTime += powf(brightnessSpeed,8) * ( ( now - lastFrameSeconds ) / (1./60));
    lastFrameSeconds = now;
}

//--------------------------------------------------------------
void testApp::draw()
{

       if( bSavePDF )
    {
        ofBeginSaveScreenAsPDF(loadedFileName+" - "+ofGetTimestampString()+".pdf", false);
        ofBackground(255);
        ofSetColor(255);
        ofPushMatrix();{

        ofTranslate(-ofGetWidth()/4.7,ofGetWidth()/180);
        float manualScaleFactor = 0.829;

        // perlin noise

        ofPushMatrix();
        {
            ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
            ofTranslate(0, -ofGetHeight()/4.0);

            float scaleFactor = tesselationRect.getWidth() / perlinNoiseImage.getWidth();
            ofScale(scaleFactor, scaleFactor);

            ofScale(manualScaleFactor,manualScaleFactor);
            ofTranslate(-perlinNoiseImage.getWidth()/2.0, -perlinNoiseImage.getHeight()/2.0);
            perlinNoiseImage.draw(0,0);

            // data

            ofTranslate(0, perlinNoiseImage.getHeight() * 3.0);

            ofScale(1.0/manualScaleFactor,1.0/manualScaleFactor); // negate the manual scale
            ofScale(1.0/scaleFactor, 1.0/scaleFactor); // negate the automatic scale

            ofSetColor(0,255);
            ofTranslate(0, -72);

            printFontHeader.drawStringAsShapes(loadedFileName,0,0);

            float textBoxWidth = (perlinNoiseImage.getWidth()*scaleFactor*0.7725*1.74);
            float columnWidth = textBoxWidth/3.0;
            float graphWidth = columnWidth * 0.7;
            float valueTextWidth = columnWidth - graphWidth ;
            float columnOffsetWidth = columnWidth + (valueTextWidth/2.0);

            ofTranslate(columnOffsetWidth, 0);

            ofSetColor(64,255);

            ofPushMatrix();
            {

                printFontHeader.drawStringAsShapes("Temperature",0,0);

                ofTranslate(0, 24);

                /* void testApp::drawSliderForPdf(string name,
                                                  float x, float y, float width, float height,
                                                  float valueMin, float valueMax,
                                                  float value,
                                                  float valueLow, float valueHigh){
                */

                drawSliderForPdf("range",
                                 0, 0, graphWidth, 12,
                                 kelvinWarm, kelvinCold,
                                 0,
                                 kelvinWarmRange, kelvinColdRange);

                ofTranslate(0, 24);

                drawSliderForPdf("speed",
                                 0, 0, graphWidth, 12,
                                 0, 1,
                                 temperatureSpeed);

                ofTranslate(0, 24);

                drawSliderForPdf("spread",
                                 0, 0, graphWidth, 12,
                                 0, 1,
                                 temperatureSpread);

            }
            ofPopMatrix();

            ofTranslate(columnOffsetWidth, 0);

            ofPushMatrix();
            {

                printFontHeader.drawStringAsShapes("Brightness",0,0);

                ofTranslate(0, 24);

                /* void testApp::drawSliderForPdf(string name,
                                                  float x, float y, float width, float height,
                                                  float valueMin, float valueMax,
                                                  float value,
                                                  float valueLow, float valueHigh){
                */

                drawSliderForPdf("range",
                                 0, 0, graphWidth, 12,
                                 0, 1,
                                 0,
                                 brightnessRangeFrom, brightnessRangeTo);

                ofTranslate(0, 24);

                drawSliderForPdf("speed",
                                 0, 0, graphWidth, 12,
                                 0, 1,
                                 brightnessSpeed);

                ofTranslate(0, 24);

                drawSliderForPdf("spread",
                                 0, 0, graphWidth, 12,
                                 0, 1,
                                 brightnessSpread);

            }
            ofPopMatrix();


        }
        ofPopMatrix();

        // tesselation

        ofPushMatrix();
        {
            ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
            ofScale(manualScaleFactor,manualScaleFactor);
            ofTranslate(45, 25, 0);

            for(std::vector<TesselationSquare*>::iterator it = tesselation.begin(); it != tesselation.end(); ++it)
            {
                TesselationSquare* t = *(it);
                t->drawForPdf();
            }

        }
        ofPopMatrix();

            }ofPopMatrix();

        ofEndSaveScreenAsPDF();
        bSavePDF = false;
    }


    ofBackgroundGradient(ofColor(40), ofColor(10), OF_GRADIENT_CIRCULAR);
    glEnable(GL_DEPTH_TEST);
    ofEnableSmoothing();
    float viewportWidth = (ofGetWidth()-gui->getRect()->getWidth());
    float scale = viewportWidth *1.15 / ofGetWidth();
    ofViewport(gui->getRect()->getWidth(),0,viewportWidth,ofGetHeight());
    cam.begin();
    ofPushMatrix();
    ofTranslate(viewportWidth/2, ofGetHeight()/2.5);
    ofScale(scale, scale, scale);
    for(std::vector<TesselationSquare*>::iterator it = tesselation.begin(); it != tesselation.end(); ++it)
    {
        TesselationSquare* t = *(it);
        t->draw();
    }
    ofTranslate(0, ofGetHeight()/3);
    ofSetColor(255);
    glDisable(GL_DEPTH_TEST);
    perlinNoiseImage.draw(tesselationRect.getMinX(),tesselationRect.getMinY(),tesselationRect.getWidth(), tesselationRect.getHeight());
    ofPopMatrix();
    cam.end();
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
    if(key == 'f')
    {
        ofToggleFullscreen();
    }
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

void testApp::drawSliderForPdf(string name, float x, float y, float width, float height,
                               float valueMin, float valueMax, float value, float valueLow, float valueHigh)
{
    ofFill();

    ofPushStyle();
    ofSetColor((ofGetStyle().color * 0.5)+127);
    ofRect(x,y,width,height);
    ofPopStyle();

    string valueString = "";

    if(valueHigh == -1 && valueLow == -1)
    {
        ofRect(x, y, ofMap(value, valueMin, valueMax, 0, width), height);
        valueString = ofToString(value);
    }
    else
    {
        ofRect(x + ofMap(valueLow, valueMin, valueMax, 0, width), y, ofMap(valueHigh, valueMin, valueMax, 0, width-ofMap(valueLow, valueMin, valueMax, 0, width)), height);
        int rounding = ((valueMax - valueMin) / 1000.0 > 1.0 )? 0 : 3;
        valueString = ofToString(valueLow, rounding) + " ... " + ofToString(valueHigh, rounding);
    }

    ofPushMatrix();

    ofTranslate(-(printFontText.stringWidth(name)+height), height);
    printFontText.drawStringAsShapes(name, 0, 0);

    ofPopMatrix();

    ofPushMatrix();

    ofTranslate(width+height, height);
    printFontText.drawStringAsShapes(valueString, 0, 0);

    ofPopMatrix();

    /*  outline

        ofNoFill();
        ofPushStyle();
        ofSetColor((ofGetStyle().color * 0.5)+127);
        ofRect(x,y,width,height);
        ofPopStyle();
    */

}
