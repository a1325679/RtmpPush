//#include "XRtmpThread.h"
//extern "C"
//{
//#include <libavformat/avformat.h>
//#include <libavutil/time.h>
//}
//XRtmpThread::XRtmpThread(){
//	if(!xr)
//		xr = XRtmp::Get();
//}
//XRtmpThread::~XRtmpThread() {
//	
//}
//bool XRtmpThread::Open(const char * url) {
//	
//	xr->Init(url);
//	return true;
//}
//void XRtmpThread::run() {
//	AVPacket* pkt;
//	while (!isExit) {
//		AVStream* instream, * outstream;
//		pkt = Pop();
//		if (pkt == nullptr) {
//			continue;
//		}
//		mux.lock();
//		instream = ifmt->streams[pkt->stream_index];
//		xr->SendFrame(pkt, instream, pkt->stream_index);
//		mux.unlock();
//
//		av_free_packet(pkt);
//	}
//}
//void XRtmpThread::Push(AVPacket* pkt)
//{
//	if (!pkt)return;
//	//阻塞
//	while (!isExit)
//	{
//		mux.lock();
//		if (packs.size() < maxList)
//		{
//			packs.push_back(pkt);
//			mux.unlock();
//			break;
//		}
//		mux.unlock();
//		msleep(1);
//	}
//}
////取出一帧数据，并出栈，如果没有返回NULL
//AVPacket* XRtmpThread::Pop()
//{
//	mux.lock();
//	if (packs.empty())
//	{
//		mux.unlock();
//		return NULL;
//	}
//	AVPacket* pkt = packs.front();
//	packs.pop_front();
//	mux.unlock();
//	return pkt;
//}