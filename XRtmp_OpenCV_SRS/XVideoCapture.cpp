#include "XVideoCapture.h"
#include <opencv2/highgui.hpp>
#include "XVideoCapture.h"
#include <iostream>
#include "XFilter.h"
#pragma comment(lib,"opencv_world320.lib")
using namespace std;
using namespace cv;


class CXVideoCapture :public XVideoCapture {
public:
	void Main() {
		Mat frame;
		while (!isExit) {
			if (!cam.read(frame)) {
				this_thread::sleep_for(1ms);
				continue;
			}
			if (frame.empty())
			{
				this_thread::sleep_for(1ms);;
				continue;
			}
			imshow("v", frame);
			waitKey(1);

			{
				unique_lock<mutex> lok_(mux);
				for (int i = 0; i < filters.size(); i++)
				{
					Mat des;
					filters[i]->Filter(&frame, &des);
					frame = des;
				}
			}
		
			XData d((char*)frame.data, frame.cols * frame.rows * frame.elemSize(), GetCurTime());
			Push(d);
		}
	}
	bool Init(int camIndex = 0)
	{
		cam.open(camIndex);
		if (!cam.isOpened())
		{
			cout << "cam open failed!" << endl;
			return false;
		}
		cout << camIndex << " cam open success" << endl;
		width = cam.get(CAP_PROP_FRAME_WIDTH);
		height = cam.get(CAP_PROP_FRAME_HEIGHT);
		fps = cam.get(CAP_PROP_FPS);
		if (fps == 0) fps = 25;
		return true;
	}
	void Stop()
	{
		XThread::Stop();
		if (cam.isOpened())
		{
			cam.release();
		}
	}
private:
	VideoCapture cam;
};

XVideoCapture* XVideoCapture::getInstance()
{
	static CXVideoCapture xc;
	return &xc;
}
XVideoCapture::XVideoCapture()
{
}


XVideoCapture::~XVideoCapture()
{
}