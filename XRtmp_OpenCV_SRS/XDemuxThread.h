#pragma once
#include <QThread>
#include "XRtmp.h"
#include <mutex>
#include"XRtmpThread.h"
class XDemux;
class XDemuxThread :public QThread
{
public:
	//创建对象并打开
	virtual bool Open(const char* url);
	//启动所有线程
	virtual void Start();
	//关闭线程清理资源
	//virtual void Close();
	void run();
	XDemuxThread();
	virtual ~XDemuxThread();

	bool isExit = false;
	std::mutex mux;
	XDemux* demux = 0;
	XRtmpThread*  xr = 0;
};

