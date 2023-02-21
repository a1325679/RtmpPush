#pragma once
#include"XThread.h"
#include"XFilter.h"
#include<vector>
class XVideoCapture:public XThread
{
public:
	int width = 0;
	int height = 0;
	int fps = 0;
	static XVideoCapture* getInstance();
	virtual bool Init(int camIndex = 0) = 0;
	virtual void Stop() = 0;
	virtual ~XVideoCapture();
	void AddFilter(XFilter* f)
	{
		mux.lock();
		filters.push_back(f);
		mux.unlock();
	}
protected:
	std::vector<XFilter*> filters;
	std::mutex mux;
	XVideoCapture();
};

