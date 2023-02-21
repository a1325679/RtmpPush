#pragma once
#include<map>
#include<iostream>
#include<string>
using namespace std;
namespace cv
{
	class Mat;
}


enum XFilterType
{
	XBILATERAL //Ë«±ßÂË²¨
};

class XFilter
{
public:
	static XFilter* Get(XFilterType t = XBILATERAL);
	virtual bool Filter(cv::Mat* src, cv::Mat* des) = 0;
	virtual bool Set(std::string key, double value);
	virtual ~XFilter();
protected:
	std::map<std::string, double>paras;
	XFilter();
};
