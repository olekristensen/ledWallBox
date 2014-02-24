#include "pgCamera.h"
#include <iostream>

pgCamera::pgCamera()
{
    PrintBuildInfo();
}

int pgCamera::connect(PGRGuid* guid)
{
    if(guid != NULL){
        error = cam.Connect(guid);
    } else {
        error = cam.Connect();
    }
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    // Get the camera information
    CameraInfo camInfo;
    error = cam.GetCameraInfo(&camInfo);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    PrintCameraInfo(&camInfo);
    return 1;
}

pgCamera::~pgCamera()
{
    //dtor
    stop();
    if(cam.IsConnected()){
        disconnect();
    }
}

int pgCamera::disconnect()
{
    // Disconnect the camera
    error = cam.Disconnect();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }
    std::cout << "Disconnected camera" << std::endl;
}

int pgCamera::stop()
{
    // Stop capturing images
    error = cam.StopCapture();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }
    std::cout << "Stopped Camera" << std::endl;
}

void pgCamera::PrintBuildInfo()
{
    FC2Version fc2Version;
    Utilities::GetLibraryVersion( &fc2Version );
    char version[128];
    sprintf(
        version,
        "FlyCapture2 library version: %d.%d.%d.%d\n",
        fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );

    printf( version );

    char timeStamp[512];
    sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );

    printf( timeStamp );
}

void pgCamera::PrintCameraInfo( CameraInfo* pCamInfo )
{
    char macAddress[64];
    sprintf(
        macAddress,
        "%02X:%02X:%02X:%02X:%02X:%02X",
        pCamInfo->macAddress.octets[0],
        pCamInfo->macAddress.octets[1],
        pCamInfo->macAddress.octets[2],
        pCamInfo->macAddress.octets[3],
        pCamInfo->macAddress.octets[4],
        pCamInfo->macAddress.octets[5]);

    char ipAddress[32];
    sprintf(
        ipAddress,
        "%u.%u.%u.%u",
        pCamInfo->ipAddress.octets[0],
        pCamInfo->ipAddress.octets[1],
        pCamInfo->ipAddress.octets[2],
        pCamInfo->ipAddress.octets[3]);

    char subnetMask[32];
    sprintf(
        subnetMask,
        "%u.%u.%u.%u",
        pCamInfo->subnetMask.octets[0],
        pCamInfo->subnetMask.octets[1],
        pCamInfo->subnetMask.octets[2],
        pCamInfo->subnetMask.octets[3]);

    char defaultGateway[32];
    sprintf(
        defaultGateway,
        "%u.%u.%u.%u",
        pCamInfo->defaultGateway.octets[0],
        pCamInfo->defaultGateway.octets[1],
        pCamInfo->defaultGateway.octets[2],
        pCamInfo->defaultGateway.octets[3]);

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
        pCamInfo->serialNumber,
        pCamInfo->modelName,
        pCamInfo->vendorName,
        pCamInfo->sensorInfo,
        pCamInfo->sensorResolution,
        pCamInfo->firmwareVersion,
        pCamInfo->firmwareBuildTime,
        pCamInfo->gigEMajorVersion,
        pCamInfo->gigEMinorVersion,
        pCamInfo->userDefinedName,
        pCamInfo->xmlURL1,
        pCamInfo->xmlURL2,
        macAddress,
        ipAddress,
        subnetMask,
        defaultGateway );
}

void pgCamera::PrintStreamChannelInfo( GigEStreamChannel* pStreamChannel )
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

void pgCamera::PrintError( Error error )
{
    error.PrintErrorTrace();
}
