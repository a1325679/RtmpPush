#pragma once
#include<list>
#include"XThread.h"
class AVPacket;
class AVCodecContext;
class AVStream;
class XRtmp
{
public:

	static XRtmp* Get();

	virtual bool Init(const char* url) = 0;

	virtual int AddStream(const AVCodecContext *c) = 0;
	virtual bool SendHead() = 0;

	virtual bool SendFrame(XData d,int streamIndex) = 0;
	virtual void Close() = 0;
	virtual ~XRtmp();
protected:
	XRtmp();
};

