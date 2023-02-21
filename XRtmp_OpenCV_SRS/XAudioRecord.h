#include "XThread.h"

enum XAUDIOTYPE
{
	X_AUDIO_QT
};
class XAudioRecord :public XThread
{
public:
	int channels = 2;		//������
	int sampleRate = 44100;	//������
	int sampleByte = 2;		//�����ֽڴ�С
	int nbSamples = 1024;	//һ֡��Ƶÿ��ͨ������������
	//��ʼ¼��
	virtual bool Init() = 0;

	//ֹͣ¼��
	virtual void Stop() = 0;

	static XAudioRecord* Get();
	virtual ~XAudioRecord();
protected:
	XAudioRecord();
};

