#ifndef _IVCP_IVCPCOS_H__
#define _IVCP_IVCPCOS_H__

#include <string>
#include <mutex>
#include "ivcpMsgStruct.h"
#include <cos_c_sdk/cos_define.h>
#include <memory>
#include <thread>
#include "ivcpCacheQueue.h"
#include "ivcpAvData.h"
#include "mpeg-ps.h"
#include "hls-m3u8.h"
#include "hls-media.h"
#include "hls-param.h"

class IvcpCosStatus {
public:
    IvcpCosStatus();
    ~IvcpCosStatus();
public:
    bool GetCosIOStatus();
    bool ReInitCosIO();

private:
    bool IOStatus;
};

class IvcpCos
{
public:
    IvcpCos(std::string streamid, std::string planid, bool needThread = true);
    IvcpCos();
    ~IvcpCos();

public:
    //从mgr过来的数据
    void OnAVData(std::shared_ptr<AvData> avData);
public:
    int UploadImage(const unsigned char* imageData, int imageDataLen, std::string& url);
    int UploadVideoSlice(const unsigned char* videoSliceData, int videoSliceDataLen, std::string& url);
    int UploadVideoSliceResume(const unsigned char* videoSliceData, int videoSliceDataLen, std::string& url);

    int TouchBucket();
    void ReSetStartTime(time_t t) { m_startTime = t; }
    time_t GetStartTime() { return m_startTime; }
    std::string GetPlanId() { return m_PlanId; }
    std::string GetStreamId() { return m_StreamId; }
private:
    //处理缓冲数据线程
    void  DealAvData();
private:
    //音视频流切片上传cos
    int UploadVideoStream(const unsigned char* videoSliceData, int videoSliceDataLen, std::string& url);
    static int HlsHandler(void* m3u8, const void* data, size_t bytes, int64_t pts, int64_t dts, int64_t duration);
private:
    std::string PreEnd;
private:
    std::string m_PlanId;
    std::string m_StreamId;
private:
    CacheQueue<std::shared_ptr<AvData>> m_avData;
    std::shared_ptr<std::thread> m_thread;
    bool m_exited;
    hls_media_t* m_hls;
    uint64_t m_startTime;
};

#endif
