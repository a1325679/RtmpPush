#pragma once
#include <QThread>
#include "XRtmp.h"
#include <mutex>
#include"XRtmpThread.h"
class XDemux;
class XDemuxThread :public QThread
{
public:
	//�������󲢴�
	virtual bool Open(const char* url);
	//���������߳�
	virtual void Start();
	//�ر��߳�������Դ
	//virtual void Close();
	void run();
	XDemuxThread();
	virtual ~XDemuxThread();

	bool isExit = false;
	std::mutex mux;
	XDemux* demux = 0;
	XRtmpThread*  xr = 0;
};

