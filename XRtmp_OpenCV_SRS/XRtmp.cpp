#include "XRtmp.h"
#include <iostream>
#include <string>
using namespace std;

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/time.h>
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib,"avutil.lib")

class CXRtmp :public XRtmp {
public:
	void Close() {
		if (ic) {
			avformat_close_input(&ic);

			vs = nullptr;
		}
		vc = nullptr;
		url = "";
	}
	bool Init(const char* url) {
		int ret = avformat_alloc_output_context2(&ic, nullptr, "flv", url);
		this->url = url;
		if (ret != 0) {
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			cout << buf << endl;
			return false;
		}
		return true;
	}
	int AddStream(const AVCodecContext*c){
		AVStream* outstream = avformat_new_stream(ic,nullptr);
		if (!outstream) {
			cout << "Failed allocating output stream" << endl;
			return -1;
		}
		outstream->codecpar->codec_tag = 0;
		int ret = avcodec_parameters_from_context(outstream->codecpar, c);
		if (c->codec_type == AVMEDIA_TYPE_VIDEO) {
			vs = outstream;
			vc = c;
		}
		else if (c->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			ac = c;
			as = outstream;
		}
		return outstream->index;
	}
	bool SendHead() {
		///��rtmp ���������IO
		int ret = avio_open(&ic->pb, url.c_str(), AVIO_FLAG_WRITE);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			cout << buf << endl;
			return false;
		}
		//д��װͷ
		ret = avformat_write_header(ic, nullptr);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			cout << buf << endl;
			return false;
		}
		return true;
	}
	bool SendFrame(XData d, int streamIndex)
	{
		if (!d.data || d.size <= 0)return false;
		AVPacket* pack = (AVPacket*)d.data;
		pack->stream_index = streamIndex;
		AVRational stime;
		AVRational dtime;

		//�ж�����Ƶ������Ƶ
		if (vs && vc && pack->stream_index == vs->index)
		{
			stime = vc->time_base;
			dtime = vs->time_base;
		}
		else if (as && ac && pack->stream_index == as->index)
		{
			stime = ac->time_base;
			dtime = as->time_base;
		}
		else
		{
			return false;
		}

		//����
		pack->pts = av_rescale_q(pack->pts, stime, dtime);
		pack->dts = av_rescale_q(pack->dts, stime, dtime);
		pack->duration = av_rescale_q(pack->duration, stime, dtime);
		int ret = av_interleaved_write_frame(ic, pack);
		if (ret == 0)
		{
			cout << "#" << flush;
			return true;
		}
		return false;
	}

private:
	//rtmp flv ��װ��
	AVFormatContext* ic = nullptr;

	//��Ƶ������
	const AVCodecContext* vc = nullptr;
	//��Ƶ������
	const AVCodecContext* ac = nullptr;

	AVStream* vs = nullptr;
	//��Ƶ��
	AVStream* as = nullptr;

	AVStream* istream = nullptr;

	string url = "";

};

//������������
XRtmp* XRtmp::Get()
{
	static CXRtmp cxr;

	static bool isFirst = true;
	if (isFirst)
	{
		//ע�����еķ�װ��
		av_register_all();

		//ע����������Э��
		avformat_network_init();
		isFirst = false;
	}	
	return &cxr;
}
XRtmp::XRtmp()
{
}


XRtmp::~XRtmp()
{
}
