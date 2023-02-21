#pragma once
#include"XRtmp.h"
#include<list>
#include<mutex>
#include <QThread>
struct AVFormatContext;
class XRtmpThread:public QThread
{
public:
	AVFormatContext* ifmt;
	XRtmpThread();
	~XRtmpThread();
	void Push(AVPacket* pkt);
	AVPacket* Pop();
	void run();
	bool Open(const char* url);

	XRtmp* xr = 0;

	std::list<AVPacket*> packs;
	bool isExit = false;
	std::mutex mux;
	int maxList = 100;
};

