#include "XRtmp_OpenCV_SRS.h"
#include <QtWidgets/QApplication>
#include<iostream>
#include <stdio.h>

#include"XDemuxThread.h"
#include"XRtmpThread.h"


#include "XRtmp.h"
#include"XController.h"
using namespace std;

int main(int argc, char *argv[])
{
	XController::Get()->outUrl = "rtmp://192.168.116.135:1935/live/temp";
	XController::Get()->Start();
	
	this_thread::sleep_for(20000ms);
	XController::Get()->Stop();
}
