#include "XThread.h"
#include <stdlib.h>
#include<thread>
#include <string.h>
extern "C"
{
#include <libavutil/time.h>
}
long long GetCurTime()
{
	return av_gettime();
}

void XData::Drop()
{
	if (data)
		delete data;
	data = nullptr;
	size = 0;
}



XData::XData(char* data, int size, long long p) {
	this->data = new char[size];
	memcpy(this->data, data, size);
	this->size = size;
	this->pts = p;
}

XData::XData() {

}
XData::~XData()
{
}

bool XThread::Start()
{
	std::unique_lock<std::mutex>lock_(mux);
	isExit = false;
	th_ = std::thread(&XThread::Main, this);
	return true;
}

void XThread::Push(XData d) {
	std::unique_lock<std::mutex> loc_(mux);
	if (datas.size() > maxList) {
		datas.front().Drop();
		datas.pop_front();
	}
	datas.push_back(d);
}

XData XThread::Pop() {
	std::unique_lock<std::mutex> loc_(mux);
	if (datas.empty())
	{
		return XData();
	}
	XData d = datas.front();

	datas.pop_front();

	return d;

}
void XThread::Wait()
{
	if (th_.joinable()) //判断子线程是否可以等待
		th_.join();     //等待子线程退出
}
//退出线程，并等待线程退出（阻塞）
void XThread::Stop()
{
	isExit = true;
	Wait();
}

void XThread::Clear() {
	std::unique_lock<std::mutex> loc_(mux);
	while (!datas.empty()) {
		datas.front().Drop();
		datas.pop_front();
	}
}


XThread::XThread()
{
}


XThread::~XThread()
{
}


