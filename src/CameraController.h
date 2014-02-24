#ifndef CAMERACONTROLLER_H_INCLUDED
#define CAMERACONTROLLER_H_INCLUDED

#pragma once

#include "FlyCapture2.h"
using namespace FlyCapture2;

#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

class CameraController
{
public:
    static const int camWidth = 1280;
    static const int camHeight = 720;

    static const float minBlobArea = .001;
    static const float maxBlobArea = .2;
    static const int maxBlobs = 8;
    static const int thresholdLevel = 128;

    const Mode k_fmt7Mode = MODE_0;
    const PixelFormat k_fmt7PixFmt = PIXEL_FORMAT_RAW8;

    const unsigned int k_HDRCtrl = 0x1800;

    const unsigned int k_HDRShutter1 = 0x1820;
    const unsigned int k_HDRShutter2 = 0x1840;
    const unsigned int k_HDRShutter3 = 0x1860;
    const unsigned int k_HDRShutter4 = 0x1880;

    const unsigned int k_HDRGain1 = 0x1824;
    const unsigned int k_HDRGain2 = 0x1844;
    const unsigned int k_HDRGain3 = 0x1864;
    const unsigned int k_HDRGain4 = 0x1884;

    const unsigned int k_HDROn = 0x82000000;
    const unsigned int k_HDROff = 0x80000000;


    vector<GigECamera*> cameras;
    vector<ofxCvColorImage*> images;
//	vector<ofxCvContourFinder*> contourFinders;

    CameraController()
    {
    }

    ~CameraController()
    {
        for(unsigned int i = 0; i < cameras.size(); i++)
        {
            cameras[i]->StopCapture();
            cameras[i]->Disconnect();
            delete cameras[i];
            delete images[i];
//			delete contourFinders[i];
        }
    }


    bool IsHDRSupported( Camera* pCam )
    {
        Error error;
        unsigned int RegVal;
        error = pCam->ReadRegister( k_HDRCtrl, &RegVal );
        if (error != PGRERROR_OK)
        {
            catchError( error );
            return false;
        }

        return ( !0x80000000 & RegVal ) ? false : true;
    }

    void ToggleHDRMode( Camera* pCam, bool hdrOn )
    {
        Error error;
        error = pCam->WriteRegister( k_HDRCtrl, hdrOn ? k_HDROn : k_HDROff );
        if (error != PGRERROR_OK)
        {
            catchError( error );
        }

        printf( "HDR mode turned to %s\n", hdrOn ? "on" : "off" );
    }

    void InitializeHDRRegisters( Camera* pCam )
    {
        Error error;
        error = pCam->WriteRegister( k_HDRShutter1, 0x000 );
        error = pCam->WriteRegister( k_HDRShutter2, 0x120 );
        error = pCam->WriteRegister( k_HDRShutter3, 0x240 );
        error = pCam->WriteRegister( k_HDRShutter4, 0x360 );

        error = pCam->WriteRegister( k_HDRGain1, 0x000 );
        error = pCam->WriteRegister( k_HDRGain2, 0x0E3 );
        error = pCam->WriteRegister( k_HDRGain3, 0x1C6 );
        error = pCam->WriteRegister( k_HDRGain4, 0x2A9 );

        printf( "Initialized HDR registers...\n" );
    }


    void catchError(Error error)
    {
        if(error != PGRERROR_OK)
            error.PrintErrorTrace();
    }

    void printInfo(const CameraInfo& cameraInfo)
    {
        char macAddress[64];
        sprintf(
            macAddress,
            "%02X:%02X:%02X:%02X:%02X:%02X",
            cameraInfo.macAddress.octets[0],
            cameraInfo.macAddress.octets[1],
            cameraInfo.macAddress.octets[2],
            cameraInfo.macAddress.octets[3],
            cameraInfo.macAddress.octets[4],
            cameraInfo.macAddress.octets[5]);

        char ipAddress[32];
        sprintf(
            ipAddress,
            "%u.%u.%u.%u",
            cameraInfo.ipAddress.octets[0],
            cameraInfo.ipAddress.octets[1],
            cameraInfo.ipAddress.octets[2],
            cameraInfo.ipAddress.octets[3]);

        char subnetMask[32];
        sprintf(
            subnetMask,
            "%u.%u.%u.%u",
            cameraInfo.subnetMask.octets[0],
            cameraInfo.subnetMask.octets[1],
            cameraInfo.subnetMask.octets[2],
            cameraInfo.subnetMask.octets[3]);

        char defaultGateway[32];
        sprintf(
            defaultGateway,
            "%u.%u.%u.%u",
            cameraInfo.defaultGateway.octets[0],
            cameraInfo.defaultGateway.octets[1],
            cameraInfo.defaultGateway.octets[2],
            cameraInfo.defaultGateway.octets[3]);

        printf(
            "\n*** CAMERA INFORMATION ***\n"
            "Serial number - %u\n"
            "Camera model - %s\n"
            "Camera vendor - %s\n"
            "Sensor - %s\n"
            "Resolution - %s\n"
            "Firmware version - %s\n"
            "Firmware build time - %s\n"
            "GigE version - %u.%u\n"
            "User defined name - %s\n"
            "XML URL 1 - %s\n"
            "XML URL 2 - %s\n"
            "MAC address - %s\n"
            "IP address - %s\n"
            "Subnet mask - %s\n"
            "Default gateway - %s\n\n",
            cameraInfo.serialNumber,
            cameraInfo.modelName,
            cameraInfo.vendorName,
            cameraInfo.sensorInfo,
            cameraInfo.sensorResolution,
            cameraInfo.firmwareVersion,
            cameraInfo.firmwareBuildTime,
            cameraInfo.gigEMajorVersion,
            cameraInfo.gigEMinorVersion,
            cameraInfo.userDefinedName,
            cameraInfo.xmlURL1,
            cameraInfo.xmlURL2,
            macAddress,
            ipAddress,
            subnetMask,
            defaultGateway );
    }


    void PrintStreamChannelInfo( GigEStreamChannel* pStreamChannel )
    {
        char ipAddress[32];
        sprintf(
            ipAddress,
            "%u.%u.%u.%u",
            pStreamChannel->destinationIpAddress.octets[0],
            pStreamChannel->destinationIpAddress.octets[1],
            pStreamChannel->destinationIpAddress.octets[2],
            pStreamChannel->destinationIpAddress.octets[3]);

        printf(
            "Network interface: %u\n"
            "Host post: %u\n"
            "Do not fragment bit: %s\n"
            "Packet size: %u\n"
            "Inter packet delay: %u\n"
            "Destination IP address: %s\n"
            "Source port (on camera): %u\n\n",
            pStreamChannel->networkInterfaceIndex,
            pStreamChannel->hostPost,
            pStreamChannel->doNotFragment == true ? "Enabled" : "Disabled",
            pStreamChannel->packetSize,
            pStreamChannel->interPacketDelay,
            ipAddress,
            pStreamChannel->sourcePort );
    }

    void load(string filename)
    {
        ofxXmlSettings cameraSettings;
        cameraSettings.loadFile(filename);

        BusManager busMgr;
        unsigned int numCameras;
        catchError(busMgr.GetNumOfCameras(&numCameras));
        for(unsigned int i = 0; i < numCameras; i++)
        {
            GigECamera* camera = new GigECamera();

            PGRGuid guid;
            CameraInfo camInfo;

            catchError(busMgr.GetCameraFromIndex(i, &guid));
            catchError(camera->Connect(&guid));
            catchError(camera->GetCameraInfo(&camInfo));
            printInfo(camInfo);

            unsigned int numStreamChannels = 0;
            catchError(camera->GetNumStreamChannels( &numStreamChannels ));
            for (unsigned int i=0; i < numStreamChannels; i++)
            {
                GigEStreamChannel streamChannel;
                catchError(camera->GetGigEStreamChannelInfo( i, &streamChannel ));
                printf( "\nPrinting stream channel information for channel %u:\n", i );
                PrintStreamChannelInfo( &streamChannel );
            }

            printf( "Querying GigE image setting information...\n" );

            GigEImageSettingsInfo imageSettingsInfo;
            catchError(camera->GetGigEImageSettingsInfo( &imageSettingsInfo ));

            GigEImageSettings imageSettings;
            imageSettings.offsetX = (imageSettingsInfo.maxWidth-camWidth)/2;
            imageSettings.offsetY = (imageSettingsInfo.maxHeight-camHeight)/2;
            imageSettings.height = camWidth;
            imageSettings.width = camHeight;
            imageSettings.pixelFormat = PIXEL_FORMAT_RAW8;

            printf( "Setting GigE image settings...\n" );

            catchError(camera->SetGigEImageSettings( &imageSettings ));

            GigEProperty packetSizeProp;
            packetSizeProp.propType = PACKET_SIZE;
            packetSizeProp.value = static_cast<unsigned int>(9000);
            catchError(camera->SetGigEProperty(&packetSizeProp));

            GigEProperty packetDelayProp;
            packetDelayProp.propType = PACKET_DELAY;
            packetDelayProp.value = 1800;
            catchError(camera->SetGigEProperty(&packetDelayProp));

            EmbeddedImageInfo embeddedInfo;
            catchError(camera->GetEmbeddedImageInfo( &embeddedInfo ));

            embeddedInfo.timestamp.onOff = true;
            embeddedInfo.gain.onOff = true;
            embeddedInfo.shutter.onOff = true;
            embeddedInfo.brightness.onOff = true;
            embeddedInfo.exposure.onOff = true;
            embeddedInfo.whiteBalance.onOff = true;
            embeddedInfo.frameCounter.onOff = true;
            embeddedInfo.strobePattern.onOff = true;
            embeddedInfo.GPIOPinState.onOff = true;
            embeddedInfo.ROIPosition.onOff = true;
            catchError(camera->SetEmbeddedImageInfo( &embeddedInfo ));

            /** STARTING HERE **/

            catchError(camera->StartCapture());

            Property brightness(BRIGHTNESS);
            brightness.autoManualMode = false;
            brightness.valueA = cameraSettings.getValue("brightness", 0);
            catchError(camera->SetProperty(&brightness));

            Property autoExposure(AUTO_EXPOSURE);
            autoExposure.onOff = true;
            autoExposure.autoManualMode = true;
            autoExposure.valueA = cameraSettings.getValue("exposure", 0);
            catchError(camera->SetProperty(&autoExposure));

            Property gamma(GAMMA);
            gamma.onOff = false;
            catchError(camera->SetProperty(&gamma));

            Property shutter(SHUTTER);
            shutter.valueA = cameraSettings.getValue("shutter", 500);
            shutter.autoManualMode = true;
            catchError(camera->SetProperty(&shutter));

            Property gain(GAIN);
            gain.valueA = cameraSettings.getValue("gain", 5); // 5 is no gain
            gain.autoManualMode = true;
            catchError(camera->SetProperty(&gain));

            Property whitebalance(WHITE_BALANCE);
            whitebalance.valueA = cameraSettings.getValue("whitebalanceRed", 512);
            whitebalance.valueB = cameraSettings.getValue("whitebalanceBlue", 512);
            whitebalance.autoManualMode = false;
            whitebalance.onOff = false;
            catchError(camera->SetProperty(&whitebalance));

            Property framerate(FRAME_RATE);
            framerate.onOff = true;
            framerate.valueA = cameraSettings.getValue("framerate", 12);
            framerate.autoManualMode = false;
            catchError(camera->SetProperty(&framerate));

            cameras.push_back(camera);

            ofxCvColorImage* image = new ofxCvColorImage();
            image->allocate(camWidth, camHeight);
            images.push_back(image);

//			contourFinders.push_back(new ofxCvContourFinder());
        }
    }

    void update()
    {
        for(unsigned int i = 0; i < cameras.size(); i++)
        {
            GigECamera& camera = *(cameras[i]);
            Image rawImage;
            catchError(camera.RetrieveBuffer(&rawImage));

            PixelFormat pixFormat;
            unsigned int rows, cols, stride;
            rawImage.GetDimensions( &rows, &cols, &stride, &pixFormat );
            rawImage.SetColorProcessing(NEAREST_NEIGHBOR);
            // Create a converted image
            Image convertedImage;

            // Convert the raw image
            catchError(rawImage.Convert( PIXEL_FORMAT_RGB8, &convertedImage ));

            memcpy(images[i]->getPixels(), convertedImage.GetData(), camWidth * camHeight  * 3);
            images[i]->flagImageChanged();
//			images[i]->threshold(thresholdLevel);

            //int n = camWidth * camHeight;
            //contourFinders[i]->findContours(*images[i], minBlobArea * n, maxBlobArea * n, maxBlobs, false);
        }
    }

    void draw()
    {
        glPushMatrix();
        float scaleFactor = ofGetWidth()/(camWidth*2.0);
        float scaleFactorY = ofGetHeight()/(camHeight*1.0);

        ofScale(scaleFactor, scaleFactor, scaleFactor);
        for(unsigned int i = 0; i < cameras.size(); i++)
        {
            images[i]->draw(0, 0);
            //contourFinders[i]->draw();
            ofTranslate(camWidth, 0);
        }
        glPopMatrix();
    }
};


#endif // CAMERACONTROLLER_H_INCLUDED
