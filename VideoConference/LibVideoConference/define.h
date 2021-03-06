#ifndef DEFINE_H
#define DEFINE_H

#include <QtWidgets/QApplication>
#include <stdlib.h>
#include <iostream>
#include "qbuffer.h"
#include "qtcpsocket.h"
#include "MyWindow.h"
#include "ServerDialog.h"

#include "ClientDialogString.h"
#include "../LibVideoConference/ClientDialog.h"
#include "../LibVideoConference/SQLService.h"
#include "../LibVideoConference/CVideoRecorder.h"
//#include "FileUI.h"
#include <process.h>
#include "../LibScreenShare/ScreenCapturer.h"
#include "../LibScreenShare/CameraCapturer.h"
#include "../LIbScreenShare/H264Codec.h"
#include "../LibScreenShare/ImageUtils.h"
#include "../LibScreenShare/RTPService.h"  //includes windows.h(winsock) so must be added last

#include "../VideoConference/CMediaPlayer.h"
#include "../VideoConference/ConferenceUI.h"
#include "process.h"
#include "windows.h"

using namespace std;
#endif