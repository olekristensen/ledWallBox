#ifndef PGCAMERA_H
#define PGCAMERA_H
#include "FlyCapture2.h"
using namespace FlyCapture2;

class pgCamera
{
public:
    pgCamera();
    virtual ~pgCamera();
    int stop();
    int disconnect();
    int connect(PGRGuid* guid = NULL);
protected:
    BusManager busMgr;
    Camera cam;
    void PrintBuildInfo();
    void PrintCameraInfo( CameraInfo* pCamInfo );
    void PrintStreamChannelInfo( GigEStreamChannel* pStreamChannel );
    void PrintError( Error error );
private:
    Error error;
};

#endif // PGCAMERA_H
