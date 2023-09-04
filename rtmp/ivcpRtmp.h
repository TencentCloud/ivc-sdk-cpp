
#ifndef _IVCP_RTMP_H__
#define _IVCP_RTMP_H__

#include <librtmp/amf.h>
#include <librtmp/rtmp.h>
#include <librtmp/rtmp_sys.h>
#include <stdio.h>
#include "ivcpMsgStruct.h"
#include "ivcpCacheQueue.h"
#include "ivcpAvData.h"
#include <memory>
#include <thread>


#define FILEBUFSIZE (1024 * 1024 * 10) //  10M

// NALU单元
typedef struct _NaluUnit
{
    int type; //264 （1 p 5 idr 7 sps  8 pps）  265 （1 p 19 idr 33 sps  34 pps 32 vps）
    int size;
    unsigned char* data;
} NaluUnit;

class IvcpRtmpStream
{
public:
    IvcpRtmpStream(void);
    ~IvcpRtmpStream();

public:
    // 连接到RTMP Server
    bool Connect(const char* url);
    // 断开连接
    void Close();
public:
    //从mgr过来的数据
    void OnAVData(std::shared_ptr<AvData> avData);


protected:
    // 发送MetaData
    bool SendH264Metadata(LPVideoMetadata lpMetaData);
    bool SendH265Metadata(LPVideoMetadata lpMetaData);
    bool SendAACMetaData(LPAudioMetadata lpMetaData);
    bool Send711AMetaData(LPAudioMetadata lpMetaData);
    bool Send711UMetaData(LPAudioMetadata lpMetaData);
    // 发送数据帧
    bool SendH264Packet(int channelid, unsigned char* data, unsigned int size, bool bIsKeyFrame, unsigned int nTimeStamp);
    bool SendH265Packet(int channelid, unsigned char* data, unsigned int size, bool bIsKeyFrame, unsigned int nTimeStamp);
    bool SendAACPacket(int channelid, const unsigned char* data, unsigned int size, unsigned int nTimeStamp);
    bool SendG711APacket(int channelid, const unsigned char* data, unsigned int size, unsigned int nTimeStamp);
    bool SendG711UPacket(int channelid, const unsigned char* data, unsigned int size, unsigned int nTimeStamp);

private:
    //处理缓冲数据线程
    void  DealAvData();
    // 发送数据
    int SendPacket(unsigned int nPacketType, unsigned char* data, unsigned int size, unsigned int nTimestamp);

private:
    RTMP* m_pRtmp;
private:
    unsigned char* m_pFileBuf;
    unsigned int m_nFileBufSize;
    unsigned int m_nCurPos;
private:
    CacheQueue<std::shared_ptr<AvData>> m_avData;
    std::shared_ptr<std::thread> m_thread;
    bool m_exited;
};

#endif