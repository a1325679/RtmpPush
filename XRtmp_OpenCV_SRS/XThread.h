#pragma once
#include<thread>
#include<mutex>
#include<list>
#include <iostream>

long long GetCurTime();


class XData {
public:
	char* data;
	int size = 0;
	long long pts = 0;
	void Drop();
	XData();
	XData(char* data, int size, long long pts);
	virtual ~XData();
};
class XThread
{
public:
	virtual void Wait();
	virtual void Push(XData d);

	virtual XData Pop();

	virtual bool Start();

	virtual void Stop();

	virtual void Clear();

	XThread();

	virtual ~XThread();

protected:
	//线程入口函数
	virtual void Main() = 0;

	int maxList = 100;
	//标志线程退出
	bool isExit = true;

	std::mutex mux;

	std::list<XData> datas;

	std::thread th_;
};

