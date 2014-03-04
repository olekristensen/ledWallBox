#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
//    sourceCamera.connect();
//	  cameraController.load("cameraSettings.xml");

    cam.setTranslationKey(' ');

    int tsqSize = 20;

    int tesselationHeight = 6;

    // construct first tesselation

    ofVec3f origin;

    int tesselation1map[] =
    {
        0,0,0,0,0,0,
        1,1,0,0,0,0,
        1,1,1,1,1,0,
        1,1,1,1,1,1,
        1,1,1,1,1,1,
        0,1,1,0,1,1,
    };
    int tesselation1width = 6;

    origin = addTesselation(origin, tsqSize, tesselation1width, tesselationHeight, tesselation1map);

    int tesselation2map[] =
    {
        0,0,0,0,0,0,
        0,1,1,0,0,0,
        1,1,1,1,1,0,
        1,1,1,1,1,1,
        1,1,1,1,1,0,
        1,0,1,1,1,0
    };
    int tesselation2width = 6;

    origin*=1.35;

    origin += addTesselation(origin, tsqSize, tesselation2width, tesselationHeight, tesselation2map);

    int tesselation3map[] =
    {
        0,0,0,1,0,0,0,0,0,
        0,0,1,1,1,0,1,0,0,
        1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,
        0,0,0,1,1,1,0,0,0
    };
    int tesselation3width = 9;

    origin*=1.25;

    origin += addTesselation(origin, tsqSize, tesselation3width, tesselationHeight, tesselation3map);


    for(std::vector<TesselationSquare*>::iterator it = tesselation.begin(); it != tesselation.end(); ++it)
    {
        TesselationSquare* t = *(it);
        t->setPosition(t->getPosition()-(origin*.5));
     //   t->rotateAround(origin, ofVec3f(0,0,0));
    }


}
ofVec3f testApp::addTesselation(ofVec3f _origin, int _size, int _width, int _height, int* _tesselationMap){

    ofVec3f offset, firstOffset;

    for (int i = 0; i < _height*_width ; i++)
    {
        int x = i%_width;
        int y = i/_width;

        ofColor c;

        if(_tesselationMap[((_width-1)-x)+(_width*y)] > 0)
        {
            c = ofColor(255,255,255,255);
        }
        else
        {
            c = ofColor(127,127,127,10);
        }

        offset = ofVec3f(-x*_size,(x+y)*_size,y*_size);

        if(y < 1) firstOffset = offset;

        TesselationSquare * tsq = new TesselationSquare();
        tsq->set(_size,_size);
        tsq->setPosition(ofVec3f(0, 0, -_size/2) + _origin + offset);
        tsq->setOrientation(ofVec3f(0,0,0));
        tsq->setup();
        tsq->setColor(c);
        tesselation.push_back(tsq);

        tsq = new TesselationSquare();
        tsq->set(_size,_size);
        tsq->setPosition(ofVec3f(_size/2,0,0) + _origin + offset);
        tsq->setOrientation(ofVec3f(0,90,0));
        tsq->setup();
        tsq->setColor(c);
        tesselation.push_back(tsq);

        tsq = new TesselationSquare();
        tsq->set(_size,_size);
        tsq->setPosition(ofVec3f(0,_size/2,0) + _origin + offset);
        tsq->setOrientation(ofVec3f(90,0,0));
        tsq->setup();
        tsq->setColor(c);
        tesselation.push_back(tsq);
    }
    return firstOffset;
}

//--------------------------------------------------------------
void testApp::update()
{
    //  cameraController.update();
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackgroundGradient(ofColor(40), ofColor(10), OF_GRADIENT_CIRCULAR);
    ofEnableLighting();
    directionalLight.enable();
    glEnable(GL_DEPTH_TEST);
    cam.begin();
    //ofDrawAxis(200);
    ofPushMatrix();
    ofRotateX(33);
    ofRotateX(90);
    ofRotateZ(45);

    for(std::vector<TesselationSquare*>::iterator it = tesselation.begin(); it != tesselation.end(); ++it)
    {
        TesselationSquare* t = *(it);
        t->draw();
    }
    ofPopMatrix();
    cam.end();
    directionalLight.disable();
    ofDisableLighting();
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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}
