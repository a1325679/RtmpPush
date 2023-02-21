#include "XAudioRecord.h"
#include <QAudioInput>
#include <iostream>
#include"XThread.h"
using namespace std;
class CXAudioRecord :public XAudioRecord {
public:
	QAudioInput* input = NULL;
	QIODevice* io = NULL;

	void Main()
	{
		std::cout << "������Ƶ¼���߳�" << std::endl;
		//һ�ζ�ȡһ֡��Ƶ���ֽ���
		int readSize = nbSamples * channels * sampleByte;
		char* buf = new char[readSize];
		while (!isExit)
		{
			//��ȡ��¼����Ƶ
			//һ�ζ�ȡһ֡��Ƶ
			if (input->bytesReady() < readSize)
			{
				std::this_thread::sleep_for(1ms);
				continue;
			}

			int size = 0;
			while (size != readSize)
			{
				int len = io->read(buf + size, readSize - size);
				if (len < 0)break;
				size += len;
			}
			if (size != readSize)
			{
				continue;
			}
			long long pts = GetCurTime();
			//�Ѿ���ȡһ֡��Ƶ
			Push(XData(buf, readSize, pts));
		}
		delete buf;
		std::cout << "�˳���Ƶ¼���߳�" << std::endl;
	}
	void Stop()
	{
		XThread::Stop();
		if (input)
			input->stop();
		if (io)
			io->close();
		input = NULL;
		io = NULL;
	}
	bool Init()
	{
		Stop();
		///1 qt��Ƶ��ʼ¼��
		QAudioFormat fmt;
		fmt.setSampleRate(sampleRate);
		fmt.setChannelCount(channels);
		fmt.setSampleSize(sampleByte * 8);
		fmt.setCodec("audio/pcm");
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);
		QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
		if (!info.isFormatSupported(fmt))
		{
			std::cout << "Audio format not support!" << std::endl;
			fmt = info.nearestFormat(fmt);
		}
		input = new QAudioInput(fmt);

		//��ʼ¼����Ƶ
		io = input->start();
		if (!io)
			return false;
		return true;
	}
};
XAudioRecord* XAudioRecord::Get()
{
	static CXAudioRecord record;
	return &record;
}
XAudioRecord::XAudioRecord()
{
}


XAudioRecord::~XAudioRecord()
{
}
