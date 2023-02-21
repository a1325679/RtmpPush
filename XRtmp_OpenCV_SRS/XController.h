#pragma once

#include "XThread.h"
#include <string>

class XController :public XThread
{
public:
	std::string outUrl;
	int camIndex = 0;
	std::string inUrl ="";
	std::string err = "";

	static XController* Get()
	{
		static XController xc;
		return &xc;
	}
	//�趨���ղ���
	virtual bool Set(std::string key, double val);
	virtual bool Start();
	virtual void Stop();
	virtual void Main();
	virtual ~XController();
protected:
	int vindex = 0; //��Ƶ������
	int aindex = 1; //��Ƶ������
	XController();
};

