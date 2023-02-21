#include "XDemux.h"
#include<iostream>
using namespace std;
extern "C" {
#include "libavformat/avformat.h"
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")


static double r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}



//�ռ���Ҫ�������ͷ� ���ͷ�AVPacket����ռ䣬�����ݿռ� av_packet_free
AVPacket* XDemux::Read()
{
	mux.lock();
	if (!iformat_ctx) //�ݴ�
	{
		mux.unlock();
		return 0;
	}
	AVPacket* pkt = av_packet_alloc();
	//��ȡһ֡��������ռ�
	int re = av_read_frame(iformat_ctx, pkt);
	if (re != 0)
	{
		mux.unlock();
		av_packet_free(&pkt);
		return 0;
	}
	//ptsת��Ϊ����
	pkt->pts = pkt->pts * (1000 * (r2d(iformat_ctx->streams[pkt->stream_index]->time_base)));
	pkt->dts = pkt->dts * (1000 * (r2d(iformat_ctx->streams[pkt->stream_index]->time_base)));
	mux.unlock();
	cout << pkt->pts << " " << flush;
	return pkt;
}


bool XDemux::Open(const char* url) {
	Close();
	AVDictionary* opts = nullptr;
	//����rtsp����tcpЭ���
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);
	//������ʱʱ��
	av_dict_set(&opts, "max_delay", "500", 0);

	int ret = avformat_open_input(&iformat_ctx, url, nullptr, &opts);
	if (ret != 0) {
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "open " << url << " failed! :" << buf << endl;
		return false;
	}
	cout << "open " << url << " success! " << endl;
	
	ret = avformat_find_stream_info(iformat_ctx,nullptr);

	//��ӡ��Ƶ����ϸ��Ϣ
	av_dump_format(iformat_ctx, 0, url, 0);

	//��ȡ��Ƶ��
	videoStream = av_find_best_stream(iformat_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	
	
	
	audioStream = av_find_best_stream(iformat_ctx, AVMEDIA_TYPE_AUDIO, -1, -1,nullptr, 0);
	return true;
}
void XDemux::Close(){
	if (!iformat_ctx) {
		return;
	}
	avformat_close_input(&iformat_ctx);
	return;
}
XDemux::XDemux()
{
	static bool isFirst = true;
	static std::mutex dmux;
	dmux.lock();
	if (isFirst)
	{
		//��ʼ����װ��
		av_register_all();
		//��ʼ������� �����Դ�rtsp rtmp http Э�����ý����Ƶ��
		avformat_network_init();
		isFirst = false;
	}
	dmux.unlock();
}

XDemux::~XDemux()
{

}
