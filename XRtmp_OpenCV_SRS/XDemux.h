#pragma once
#include<mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecContext;
class XDemux
{
public:
	virtual bool Open(const char* url);
	virtual AVPacket* Read();
	virtual void Close();
	XDemux();
	~XDemux();

	std::mutex mux;
	AVFormatContext* iformat_ctx = nullptr;

	int videoStream = 0;
	int audioStream = 1;
};

