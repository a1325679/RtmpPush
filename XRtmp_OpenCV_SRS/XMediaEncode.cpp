#include "XMediaEncode.h"

#include <iostream>
using namespace std;
extern "C"
{
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
}

#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib,"swresample.lib")

class CXMediaEncode :public XMediaEncode
{
public:

	//初始化像素格式转换的上下文初始化
	virtual bool InitScale() {
		vsc = sws_getCachedContext(vsc, inWidth, inHeight,(AVPixelFormat)AV_PIX_FMT_BGR24,outWidth,outHeight, (AVPixelFormat)AV_PIX_FMT_YUV420P, SWS_BICUBIC,  // 尺寸变化使用算法
			0, 0, 0);
		if (!vsc) {
			cout << "sws_getCachedContext failed!";
			return false;
		}
		yuv = av_frame_alloc();
		yuv->format = AV_PIX_FMT_YUV420P;
		yuv->width = inWidth;
		yuv->height = inHeight;
		yuv->pts = 0;

		// 分配yuv空间
		int ret = av_frame_get_buffer(yuv,32);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw exception(buf);
		}
		return true;
	}

	//音频重采样上下文初始化
	virtual bool InitResample() {
		asc = NULL;
		asc = swr_alloc_set_opts(asc,
			av_get_default_channel_layout(channels), (AVSampleFormat)outSampleFmt, sampleRate,//输出格式
			av_get_default_channel_layout(channels), (AVSampleFormat)inSampleFmt, sampleRate, 0, 0);//输入格式
		if (!asc)
		{
			cout << "swr_alloc_set_opts failed!";
			return false;
		}
		int ret = swr_init(asc);
		if (ret != 0)
		{
			char err[1024] = { 0 };
			av_strerror(ret, err, sizeof(err) - 1);
			cout << err << endl;
			return false;
		}

		cout << "音频重采样 上下文初始化成功!" << endl;


		///3 音频重采样输出空间分配
		pcm = av_frame_alloc();
		pcm->format = outSampleFmt;
		pcm->channels = channels;
		pcm->channel_layout = av_get_default_channel_layout(channels);
		pcm->nb_samples = nbSamples; //一帧音频一通道的采用数量
		ret = av_frame_get_buffer(pcm, 0); // 给pcm分配存储空间
		if (ret != 0)
		{
			char err[1024] = { 0 };
			av_strerror(ret, err, sizeof(err) - 1);
			cout << err << endl;
			return false;
		}
		return true;
	}

	//返回值无需调用者清理
	virtual XData Resample(XData d) {

		XData r;
		const uint8_t* indata[AV_NUM_DATA_POINTERS] = { 0 };
		indata[0] = (uint8_t*)d.data;
		int len = swr_convert(asc, pcm->data, pcm->nb_samples, //输出参数，输出存储地址和样本数量
			indata, pcm->nb_samples
		);
		if (len <= 0)
		{
			return r;
		}
		pcm->pts = d.pts;
		r.data = (char*)pcm;
		r.size = pcm->nb_samples * pcm->channels * 2;
		r.pts = d.pts;
		return r;
	}

	//返回值无需调用者清理
	virtual XData RGBToYUV(XData d) {
		XData r;
		r.pts = d.pts;
		//输入的数据结构
		uint8_t* indata[AV_NUM_DATA_POINTERS] = {0};
		//indata[0] bgrbgrbgr
		//plane indata[0] bbbbb indata[1]ggggg indata[2]rrrrr 
		indata[0] = (uint8_t*)d.data;
		int insize[AV_NUM_DATA_POINTERS] = { 0 };
		//一行（宽）数据的字节数
		insize[0] = inWidth * inPixSize;
		
		int h = sws_scale(vsc, indata, insize, 0, inHeight, yuv->data, yuv->linesize);
		if (h <= 0)
		{
			return r;
		}
		yuv->pts = d.pts;
		r.data = (char*)yuv;
		int* p = yuv->linesize;
		while ((*p))
		{
			r.size += (*p) * outHeight;
			p++;
		}
		return r;
	}
	AVCodecContext* CreateCodec(AVCodecID cid)
	{
		AVCodec* codec = avcodec_find_encoder(cid);
		if (!codec)
		{
			cout << "avcodec_find_encoder  failed!" << endl;
			return false;
		}
		//音频编码器上下文
		AVCodecContext* c = avcodec_alloc_context3(codec);
		if (!c)
		{
			cout << "avcodec_alloc_context3  failed!" << endl;
			return false;
		}
		cout << "avcodec_alloc_context3 success!" << endl;

		c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		c->thread_count = 16;
		c->time_base = { 1,1000000 };
		return c;
	}
	bool OpenCodec(AVCodecContext** c)
	{
		//打开音频编码器
		int ret = avcodec_open2(*c, 0, 0);
		if (ret != 0)
		{
			char err[1024] = { 0 };
			av_strerror(ret, err, sizeof(err) - 1);
			cout << err << endl;
			avcodec_free_context(c);
			return false;
		}
		cout << "avcodec_open2 success!" << endl;
		return true;
	}
	//视频编码器初始化
	virtual bool InitVideoCodec() {
		///4 初始化编码上下文
		//a 找到编码器
		if (!(vc = CreateCodec(AV_CODEC_ID_H264)))
		{
			return false;
		}
		vc->bit_rate = 50 * 1024 * 8;//压缩后每秒视频的bit位大小 50kB
		vc->width = outWidth;
		vc->height = outHeight;
		vc->framerate = { fps,1 };
		//画面组的大小，多少帧一个关键帧
		vc->gop_size = 50;
		vc->max_b_frames = 0;
		vc->pix_fmt =(AVPixelFormat)AV_PIX_FMT_YUV420P;
		return OpenCodec(&vc);
	}

	//音频编码初始化
	virtual bool InitAudioCode() {
		if (!(ac = CreateCodec(AV_CODEC_ID_AAC)))
		{
			return false;
		}
		ac->bit_rate = 40000;
		ac->sample_rate = sampleRate;
		ac->sample_fmt = AV_SAMPLE_FMT_FLTP;
		ac->channels = channels;
		ac->channel_layout = av_get_default_channel_layout(channels);
		return OpenCodec(&ac);
	}

	XData EncodeVideo(XData frame)
	{
		av_packet_unref(&vpack);
		XData r;
		///h264编码
		if (frame.size <= 0 || !frame.data)return r;
		AVFrame* p = (AVFrame*)frame.data;

		int ret = avcodec_send_frame(vc, p);
		if (ret != 0)
			return r;

		ret = avcodec_receive_packet(vc, &vpack);
		if (ret != 0 || vpack.size <= 0)
			return r;
		r.data = (char*)&vpack;
		r.size = vpack.size;
		r.pts = frame.pts;
		return r;
	}

	long long lasta = -1;
	//音频编码 返回值无需调用者清理
	virtual XData EncodeAudio(XData frame) {

		XData r;
		if(frame.size <= 0 || !frame.data)return r;
		AVFrame* p = (AVFrame*)frame.data;
		if (lasta == p->pts)
		{
			p->pts += 1000;
		}
		lasta = p->pts;
		int ret = avcodec_send_frame(ac, p);
		if (ret != 0)
			return r;
		av_packet_unref(&apack);
		ret = avcodec_receive_packet(ac, &apack);
		if (ret != 0)
			return r;
		r.data = (char*)&apack;
		r.size = apack.size;
		r.pts = frame.pts;
		return r;
	}
	virtual void Close() {
		if (vsc)
		{
			sws_freeContext(vsc);
			vsc = NULL;
		}
		if (asc)
		{
			swr_free(&asc);
		}
		if (yuv)
		{
			av_frame_free(&yuv);
		}
		if (vc)
		{
			avcodec_free_context(&vc);
		}

		if (pcm)
		{
			av_frame_free(&pcm);
		}

		vpts = 0;
		av_packet_unref(&apack);
		apts = 0;
		av_packet_unref(&vpack);
	}
private:
	SwsContext* vsc = NULL;		//像素格式转换上下文
	SwrContext* asc = NULL;	//音频重采样上下文
	AVFrame* yuv = NULL;		//输出的YUV
	AVFrame* pcm = NULL;		//重采样输出PCM
	AVPacket vpack = { 0 };		//视频帧
	AVPacket apack = { 0 };		//音频帧
	int vpts = 0;
	int apts = 0;
};

XMediaEncode* XMediaEncode::Get()
{

	static bool isFirst = true;
	if (isFirst)
	{
		//注册所有的编解码器
		avcodec_register_all();
		isFirst = false;
	}

	static CXMediaEncode cxm;
	return &cxm;
}

XMediaEncode::XMediaEncode()
{
}


XMediaEncode::~XMediaEncode()
{
}