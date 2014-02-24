#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
//    sourceCamera.connect();
	cameraController.load("cameraSettings.xml");
}

//--------------------------------------------------------------
void testApp::update()
{
    cameraController.update();
}

//--------------------------------------------------------------
void testApp::draw()
{
	ofBackground(0, 0, 0);
    cameraController.draw();
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
