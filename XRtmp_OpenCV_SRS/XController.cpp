#include "XController.h"
#include "XVideoCapture.h"
#include "XAudioRecord.h"
#include "XMediaEncode.h"
#include "XRtmp.h"
#include <iostream>
using namespace std;



bool XController::Set(std::string key, double val)
{
	XFilter::Get()->Set(key, val);
	return true;
}
void XController::Main()
{
	long long beginTime = GetCurTime();
	while (!isExit)
	{
		//һ�ζ�ȡһ֡��Ƶ
		XData ad = XAudioRecord::Get()->Pop();
		XData vd = XVideoCapture::getInstance()->Pop();


		if (vd.size <= 0)
		{
			this_thread::sleep_for(1ms);
			continue;
		}
		////������Ƶ
		if (ad.size > 0)
		{
			ad.pts = ad.pts - beginTime;

			//�ز���Դ����
			XData pcm = XMediaEncode::Get()->Resample(ad);
			ad.Drop();
			XData pkt = XMediaEncode::Get()->EncodeAudio(pcm);
			if (pkt.size > 0)
			{
				////����
				if (XRtmp::Get()->SendFrame(pkt,aindex))
				{
					cout << "#" << flush;
				}
			}
		}
		//������Ƶ
		if (vd.size > 0)
		{
			vd.pts = vd.pts - beginTime;
			XData yuv = XMediaEncode::Get()->RGBToYUV(vd);
			vd.Drop();
			XData pkt = XMediaEncode::Get()->EncodeVideo(yuv);
			if (pkt.size > 0)
			{
				////����
				if (XRtmp::Get()->SendFrame(pkt, vindex))
				{
					cout << "@" << flush;
				}
			}
		}
	}
}



bool XController::Start()
{
	///1 ����ĥƤ������
	Set("d", 9.0);
	XVideoCapture::getInstance()->AddFilter(XFilter::Get());
	cout << "1 ����ĥƤ������" << endl;


	///2 �����
	if (camIndex == 0)
	{
		if (!XVideoCapture::getInstance()->Init(camIndex))
		{
			err = "2��ϵͳ���ʧ��";
			cout << err << endl;
			return false;
		}
	}
	else if (!inUrl.empty())
	{
		//if (!XVideoCapture::getInstance()->Init(inUrl.c_str()))
		//{
		//	err = "2��";
		//	err += inUrl;
		//	err += "���ʧ��";
		//	cout << err << endl;
		//	return false;
		//}
	}
	else
	{
		err = "2�������������";
		cout << err << endl;
		return false;
	}
	cout << "2����򿪳ɹ�" << endl;

	//3 qt��Ƶ��ʼ¼��
	if (!XAudioRecord::Get()->Init())
	{
		err = "3¼���豸��ʧ��";
		cout << err << endl;
		return false;
	}
	cout << "3¼���豸�򿪳ɹ�" << endl;

	///11 ��������Ƶ¼���߳�
	XAudioRecord::Get()->Start();
	XVideoCapture::getInstance()->Start();

	///����Ƶ������
	///4 ��ʼ����ʽת��������
	///��ʼ����������ݽṹ
	XMediaEncode::Get()->inWidth = XVideoCapture::getInstance()->width;
	XMediaEncode::Get()->inHeight = XVideoCapture::getInstance()->height;
	XMediaEncode::Get()->outWidth = XVideoCapture::getInstance()->width;
	XMediaEncode::Get()->outHeight = XVideoCapture::getInstance()->height;
	if (!XMediaEncode::Get()->InitScale())
	{
		err = "4��Ƶ���ظ�ʽת����ʧ��!";
		cout << err << endl;
		return false;
	}
	cout << "4��Ƶ���ظ�ʽת���򿪳ɹ�!" << endl;
	///5 ��Ƶ�ز��� �����ĳ�ʼ��
	XMediaEncode::Get()->channels = XAudioRecord::Get()->channels;
	XMediaEncode::Get()->nbSamples = XAudioRecord::Get()->nbSamples;
	XMediaEncode::Get()->sampleRate = XAudioRecord::Get()->sampleRate;
	if (!XMediaEncode::Get()->InitResample())
	{
		err = "5��Ƶ�ز��������ĳ�ʼ��ʧ��!";
		cout << err << endl;
		return false;
	}
	cout << "5��Ƶ�ز��������ĳ�ʼ���ɹ�!" << endl;
	///6��ʼ����Ƶ������
	if (!XMediaEncode::Get()->InitAudioCode())
	{
		err = "6��ʼ����Ƶ������ʧ��!";
		cout << err << endl;
		return false;
	}
	///7��ʼ����Ƶ������
	if (!XMediaEncode::Get()->InitVideoCodec())
	{
		err = "7��ʼ����ƵƵ������ʧ��!";
		cout << err << endl;
		return false;
	}
	cout << "7����Ƶ��������ʼ���ɹ�!" << endl;
	/// 8���������װ��������
	if (!XRtmp::Get()->Init(outUrl.c_str()))
	{
		string err = "8���������װ��������ʧ��!";
		cout << err << endl;
		return false;
	}
	cout << "8���������װ�������ĳɹ�!" << endl;

	///9 �������Ƶ��
	vindex = XRtmp::Get()->AddStream(XMediaEncode::Get()->vc);
	aindex = XRtmp::Get()->AddStream(XMediaEncode::Get()->ac);
	if (vindex < 0 || aindex < 0)
	{
		err = "9�������Ƶ��ʧ��!";
		cout << err << endl;
		return false;
	}
	if (vindex < 0)
	{
		err = "9�������Ƶ��ʧ��!";
		cout << err << endl;
		return false;
	}
	cout << "9�������Ƶ���ɹ�!" << endl;

	///10 ���ͷ�װͷ
	if (!XRtmp::Get()->SendHead())
	{
		err = "10 ���ͷ�װͷʧ��!";
		cout << err << endl;
		return false;
	}
	XAudioRecord::Get()->Clear();
	XVideoCapture::getInstance()->Clear();
	XThread::Start();
	return true;
}
void XController::Stop()
{
	XThread::Stop();
	XAudioRecord::Get()->Stop();
	XVideoCapture::getInstance()->Stop();
	XMediaEncode::Get()->Close();
	XRtmp::Get()->Close();
	camIndex = -1;
	inUrl = "";
	return;
}

XController::XController()
{
}


XController::~XController()
{
}
