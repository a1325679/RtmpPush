//#include "XDemuxThread.h"
//#include "XDemux.h"
//#include <iostream>
//extern "C" {
//#include "libavformat/avformat.h"
//#include "libavutil/time.h"
//}
//#pragma comment(lib,"avformat.lib")
//bool XDemuxThread::Open(const char* url) {
//	if (url == 0 || url[0] == '\0') {
//		return false;
//	}
//	if (!demux) demux = new XDemux();
//	//打开解封装
//	bool re = demux->Open(url);
//	demux->iformat_ctx;
//	if (!re)
//	{
//		std::cout << "demux->Open(url) failed!" << std::endl;
//		return false;
//	}
//
//	if (!xr)xr = new XRtmpThread();
//	re = xr->Open("rtmp://192.168.116.135:1935/live/index");
//	if (!re)
//	{
//		std::cout << "XRtmpThread->Open(url) failed!" << std::endl;
//		return false;
//	}
//
//
//	xr->xr->AddStream(demux->iformat_ctx->streams[demux->videoStream]);
//	xr->xr->AddStream(demux->iformat_ctx->streams[demux->audioStream]);
//
//
//
//	xr->xr->SendHead();
//	xr->ifmt = demux->iformat_ctx;
//
//
//
//	return re;
//}
////启动所有线程
//void XDemuxThread::Start()
//{
//	mux.lock();
//	//启动当前线程
//	if (!demux) demux = new XDemux();
//	if (!xr)xr = new XRtmpThread();
//	QThread::start();
//	xr->start();
//	mux.unlock();
//}
//void XDemuxThread::run() {
//	int  frame_index = 0;
//	int64_t start_time = av_gettime();
//	while (!isExit) {
//		mux.lock();
//		if (!demux) {
//			mux.unlock();
//			continue;
//		}
//		AVPacket* pkt = demux->Read();
//		if (!pkt)
//		{
//			mux.unlock();
//			msleep(5);
//			continue;
//		}
//		if (pkt->pts == ((int64_t)UINT64_C(0x8000000000000000))) {//pts没有赋值
//			        AVRational time_base = demux->iformat_ctx->streams[demux->videoStream]->time_base;
//			        int64_t calc_duration = (double)1000000 / av_q2d(demux->iformat_ctx->streams[demux->videoStream]->r_frame_rate);
//
//			       //Parameters
//			        pkt->pts = (double)(frame_index * calc_duration) / (double)(av_q2d(time_base) * 1000000);
//			        pkt->dts = pkt->pts;
//			        pkt->duration = (double)calc_duration / (double)(av_q2d(time_base) * 1000000);
//
//			    }
//
//			    if (pkt->stream_index == demux->videoStream) {
//			        AVRational time_base = demux->iformat_ctx->streams[demux->videoStream]->time_base;
//			        AVRational time_base_q = { 1,1000000 };
//			        int64_t pts_time = av_rescale_q(pkt->dts, time_base, time_base_q);
//			        int64_t now_time = av_gettime() - start_time;
//			        if (pts_time > now_time)
//			           av_usleep(pts_time - now_time);
//		}
//		xr->Push(pkt);
//		mux.unlock();
//		if (pkt->stream_index == demux->videoStream) {
//			   printf("Send %8d video frames to output URL\n", frame_index);
//			   frame_index++;
//		}
//		msleep(1);
//	}
//}
//
//XDemuxThread::XDemuxThread()
//{
//}
//
//
//XDemuxThread::~XDemuxThread()
//{
//	wait();
//}