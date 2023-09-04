#include "ivcpData.h"
#include "ivcpMsgType.h"
#include <glog/logging.h>
#include "flv-header.h"
#include "flv-proto.h"
#include "ot.h"

#define FLV_HEADER_SIZE 9      // DataOffset included
#define FLV_TAG_HEADER_SIZE 11 // StreamID included



IvcpData::IvcpData() : SvrPort("11850"), SvrDomainName("ivcp.ivc.myqcloud.com"), SvrIp("")
{
    RpcObj = new IvcpRpcObj();
    RegisterStatus = false;
    KeepAliveCounter = 0;
}

IvcpData::~IvcpData()
{
    /*
        1、线程退出标志置为true
        2、等待线程退出
        3、停止定时任务
        4、删除rpc实例
    */
    google::ShutdownGoogleLogging();

    delete RpcObj;

    {
        std::lock_guard<std::mutex> lk(VideoMetaMutex);
        for (auto iter : _VideoMetadata)
        {
            FreeVideoMeta(iter.second);
        }
    }
    {
        std::lock_guard<std::mutex> lk(AudioMetaMutex);
        for (auto iter : _AudioMetadata)
        {
            FreeAudioMeta(iter.second);
        }
    }
}

std::string IvcpData::GetIpByName(std::string name)
{
    struct hostent* host = gethostbyname(name.c_str());
    if (host == NULL)
        return "";
    struct in_addr in;
    in.s_addr = *((uint32_t*)host->h_addr_list[0]);

    return std::string(inet_ntoa(in));
}

std::string IvcpData::GetSvrPort()
{
    return SvrPort;
}

int IvcpData::IvcpDataInit(const char* deviceSN)
{
    /*
     1、设备序列号
     2、启动定时任务
     */
    DeviceSn = std::string(deviceSN);

    RpcObj->StartTask();

    Test();
    return 0;
}

void IvcpData::Test()
{
    //CreateRtmp(1, "rtmp://127.0.0.1/live");
   /* bool ret = false;
    do {
        ret = CreateWriteCos(1, "streamid", "planid");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 5));

    } while (ret == false);*/
}

std::string IvcpData::GetDeviceSn()
{
    return DeviceSn;
}

void IvcpData::RegisterCallback(void (*p)(const void*, int))
{
    Callback = p;
}

int IvcpData::MsgToClient(void* msg, int msgType)
{
    if (Callback)
    {
        Callback(msg, msgType);
    }
}

int IvcpData::OnCallBack(const char* requstID, int msgType, const void* msgStruct)
{
    return RpcObj->OnCallBack(requstID, msgType, msgStruct);
}

void IvcpData::OnSdkInternalMsg(std::string requestID, int msgType, int status, std::string msg)
{
    RpcObj->OnSdkInternalMsg(requestID, msgType, status, msg);
}

bool IvcpData::GetRegisterStatus()
{
    std::lock_guard<std::mutex> l(RegisterMutex);
    return RegisterStatus;
}
void IvcpData::SetRegisterStatus(bool status)
{
    LOG(INFO) << "SetRegisterStatus, status: " << status;
    std::lock_guard<std::mutex> l(RegisterMutex);

    RegisterStatus = status;
}

int IvcpData::GetKeepAliveCounter()
{
    return KeepAliveCounter;
}
void IvcpData::SetKeepAliveCounter(int count)
{
    KeepAliveCounter = count;
}

bool IvcpData::PeekSvrInfo()
{
    if (SvrIp.empty())
    {
        return false;
    }
    return true;
}

void IvcpData::SetSvrInfo(std::string data)
{
    SvrIp = data;
}

std::string IvcpData::GetSvrIp()
{
    return SvrIp;
}

void IvcpData::OnRpcFailed(std::string caller, grpc::Status& status)
{
    LOG(INFO) << "OnRpcFailed, caller:" << caller << " error_code: " << status.error_code()
        << ", error_message: " << status.error_message();

    switch (status.error_code())
    {
        // 连接服务器失败
    case grpc::UNAVAILABLE:
        SetRegisterStatus(false);
        SetKeepAliveCounter(0);
        SetSvrInfo("");
        break;

    default:
        break;
    }
}

bool IvcpData::CreateWriteCos(int channelid, std::string streamid, std::string planid)
{
    return CosMgr.CreateStreamToCos(channelid, streamid, planid);
}

void IvcpData::CloseWriteCos(int channelid)
{
    CosMgr.CloseStreamToCos(channelid);
}

void IvcpData::CloseRtmp(int channelID)
{
    RtmpMgr.CloseRtmp(channelID);
}

bool IvcpData::CreateRtmp(int channelID, std::string url)
{
    return RtmpMgr.CreateRtmp(channelID, url);
}

bool ReadNaluFromBuffer(const unsigned char* data, int dataLen, int& cur, const unsigned char** naluData, int& naluSize, int& naluType)
{
    int i = cur;
    while (i < dataLen)
    {
        if (data[i++] == 0x00 && data[i++] == 0x00)
        {
            if (data[i++] == 0x01) // 第一个nalu是三字节头
            {
                int pos = i;
                while (pos < dataLen)
                {
                    if (data[pos++] == 0x00 && data[pos++] == 0x00)
                    {
                        if (data[pos++] == 0x01) // 下一个三字节头
                        {
                            naluSize = (pos - 3) - i + 3;
                            cur = pos - 3;
                            naluType = data[i] & 0x1f;
                            *naluData = &data[i - 3];
                            return true;
                        }
                        else
                        {
                            pos--;
                            if (data[pos++] == 0x00 && data[pos++] == 0x01) // 下一个四字节头
                            {
                                naluSize = (pos - 4) - i + 3;
                                cur = pos - 4;
                                naluType = data[i] & 0x1f;
                                *naluData = &data[i - 3];
                                return true;
                            }
                        }
                    }
                }
                if (pos >= dataLen) //说明是最后一个nalu（三字节头）
                {
                    naluSize = pos - i + 3;
                    cur = dataLen;
                    naluType = data[i] & 0x1f;
                    *naluData = &data[i - 3];
                    return true;
                }
            }
            else
            {
                i--;
                if (data[i++] == 0x00 && data[i++] == 0x01) // 第一个nalu是四字节头
                {
                    int pos = i;
                    while (pos < dataLen)
                    {
                        if (data[pos++] == 0x00 && data[pos++] == 0x00)
                        {
                            if (data[pos++] == 0x01) // 下一个三字节头
                            {
                                naluSize = (pos - 3) - i + 4;
                                cur = pos - 3;
                                naluType = data[i] & 0x1f;
                                *naluData = &data[i - 4];
                                return true;
                            }
                            else
                            {
                                pos--;
                                if (data[pos++] == 0x00 && data[pos++] == 0x01) // 下一个四字节头
                                {
                                    naluSize = (pos - 4) - i + 4;
                                    cur = pos - 4;
                                    naluType = data[i] & 0x1f;
                                    *naluData = &data[i - 4];
                                    return true;
                                }
                            }
                        }
                    }
                    if (pos >= dataLen)//说明是最后一个nalu（四字节头）
                    {
                        naluSize = pos - i + 4;;
                        cur = dataLen;
                        naluType = data[i] & 0x1f;
                        *naluData = &data[i - 4];
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
void IvcpData::OnAvData(int channelID, const unsigned char* data, int dataLen, int dataType, unsigned int dts, unsigned int pts)
{
    bool key = false;

    switch (dataType)
    {
    case H264:
    {
        int cur = 0;
        const unsigned char* naluData;
        int naluSize = 0;
        int naluType = -1;

        do {
            naluData = nullptr;
            naluSize = 0;
            naluType = -1;

            bool ret = ReadNaluFromBuffer(data, dataLen, cur, &naluData, naluSize, naluType);
            if (!ret)continue;

            if (naluType == 0x05) // i frame
            {
                key = true;
                std::shared_ptr<AvData> avData(new AvData(channelID, naluData, naluSize, dataType, dts, pts, key));
                RtmpMgr.OnAVData(avData);
                CosMgr.OnAVData(avData);
                P2PMgr.OnAVData(avData);
            }
            else if (naluType == 0x06) //sei
            {
            }
            else if (naluType == 0x07) ///sps
            {
                VideoMetadata metadata;
                metadata.Sps = metadata.Pps = metadata.Vps = nullptr;
                metadata.nSpsLen = naluSize;
                metadata.Sps = (unsigned char*)naluData;

                /*for (int i = 0; i < naluSize;i++)
                {
                    static int s = 0;
                    if (s % 10 == 0)
                    {
                        printf("\n");
                    }
                    s++;
                    std::cout << std::hex << (int)metadata.Sps[i] << " " << std::flush;
                }
                printf("\n\n\n\n");*/
                OnVideoMetaData(channelID, &metadata);
                {
                    std::shared_ptr<AvData> avData(new AvData(channelID, naluData, naluSize, dataType, dts, pts, key));
                    CosMgr.OnAVData(avData);
                    P2PMgr.OnAVData(avData);
                }

            }
            else if (naluType == 0x08) /// pps
            {
                VideoMetadata metadata;
                metadata.Sps = metadata.Pps = metadata.Vps = nullptr;
                metadata.nPpsLen = naluSize;
                metadata.Pps = (unsigned char*)naluData;

                /* for (int i = 0; i < naluSize;i++)
                 {
                     static int s = 0;
                     if (s % 10 == 0)
                     {
                         printf("\n");
                     }
                     s++;
                     std::cout << std::hex << (int)metadata.Pps[i] << " " << std::flush;
                 }
                 printf("\n\n\n\n");*/
                OnVideoMetaData(channelID, &metadata);
                {
                    std::shared_ptr<AvData> avData(new AvData(channelID, naluData, naluSize, dataType, dts, pts, key));
                    CosMgr.OnAVData(avData);
                    P2PMgr.OnAVData(avData);
                }
            }
            else
            {
                key = false;
                std::shared_ptr<AvData> avData(new AvData(channelID, naluData, naluSize, dataType, dts, pts, key));
                RtmpMgr.OnAVData(avData);
                CosMgr.OnAVData(avData);
                P2PMgr.OnAVData(avData);
            }
        } while (cur < dataLen);
    }
    break;
    case H265:
    {
        /* int i = 0;
         while (i < dataLen)
         {
             if (data[i++] == 0x00 && data[i++] == 0x00)
             {
                 if (data[i++] == 0x01) // 三字节头
                 {
                     frameType = data[i] & 0x1f;
                     break;
                 }
                 else
                 {
                     i--;
                     if (data[i++] == 0x00 && data[i++] == 0x01) // 四字节头
                     {
                         frameType = data[i] & 0x1f;
                         break;
                     }
                     else
                     {
                         LOG(INFO) << "OnAvData not 265 header:";

                         return;
                     }
                 }
             }
         }

         if (frameType == 0x06) // i frame
         {
             key = true;
         }
         else if (frameType == 0x07) ///sps pps
         {
             VideoMetadata metadata;
             metadata.Sps = metadata.Pps = metadata.Vps = nullptr;
             metadata.nSpsLen = dataLen;
             metadata.Sps = (unsigned char*)data;

             OnVideoMetaData(channelID, &metadata);
             return;
         }
         else if (frameType == 0x08) ///sps pps
         {
             VideoMetadata metadata;
             metadata.Sps = metadata.Pps = metadata.Vps = nullptr;
             metadata.nPpsLen = dataLen;
             metadata.Pps = (unsigned char*)data;

             OnVideoMetaData(channelID, &metadata);
             return;
         }
         else if (frameType == 0x09) ///sps pps
         {
             VideoMetadata metadata;
             metadata.Sps = metadata.Pps = metadata.Vps = nullptr;
             metadata.nPpsLen = dataLen;
             metadata.Pps = (unsigned char*)data;

             OnVideoMetaData(channelID, &metadata);
             return;
         }*/
    }
    break;
    default:
    {
        std::shared_ptr<AvData> avData(new AvData(channelID, data, dataLen, dataType, dts, pts, key));

        RtmpMgr.OnAVData(avData);
        CosMgr.OnAVData(avData);
        P2PMgr.OnAVData(avData);
    }
    break;
    }
}

void IvcpData::OnVideoMetaData(int channelID, const LPVideoMetadata videoMetaData)
{
    if (videoMetaData)
    {
        std::lock_guard<std::mutex> lk(VideoMetaMutex);

        auto metaData = _VideoMetadata.find(channelID);
        if (metaData != _VideoMetadata.end())
        {
            if (videoMetaData->Sps)
            {
                if (metaData->second->Sps)
                {
                    free(metaData->second->Sps);
                }
                metaData->second->nSpsLen = videoMetaData->nSpsLen;
                metaData->second->Sps = (unsigned char*)malloc(sizeof(unsigned char) * videoMetaData->nSpsLen);
                memcpy(metaData->second->Sps, videoMetaData->Sps, videoMetaData->nSpsLen);

            }
            if (videoMetaData->Pps)
            {
                if (metaData->second->Pps)
                {
                    free(metaData->second->Pps);
                }
                metaData->second->nPpsLen = videoMetaData->nPpsLen;
                metaData->second->Pps = (unsigned char*)malloc(sizeof(unsigned char) * videoMetaData->nPpsLen);
                memcpy(metaData->second->Pps, videoMetaData->Pps, videoMetaData->nPpsLen);

            }
            if (videoMetaData->Vps)
            {
                if (metaData->second->Vps)
                {
                    free(metaData->second->Vps);
                }
                metaData->second->nVpsLen = videoMetaData->nVpsLen;
                metaData->second->Vps = (unsigned char*)malloc(sizeof(unsigned char) * videoMetaData->nVpsLen);
                memcpy(metaData->second->Vps, videoMetaData->Vps, videoMetaData->nVpsLen);

            }
        }
    }
}

void IvcpData::OnAvMetaData(int channelID, const LPVideoMetadata videoMetaData, const LPAudioMetadata audioMetaData)
{
    if (videoMetaData)
    {
        std::lock_guard<std::mutex> lk(VideoMetaMutex);

        auto metaData = _VideoMetadata.find(channelID);
        if (metaData != _VideoMetadata.end())
        {
            metaData->second->nWidth = videoMetaData->nWidth;
            metaData->second->nHeight = videoMetaData->nHeight;
            metaData->second->nFrameRate = videoMetaData->nFrameRate;
        }
        else
        {
            LPVideoMetadata tMetaData = (LPVideoMetadata)malloc(sizeof(VideoMetadata));

            tMetaData->nFrameRate = videoMetaData->nFrameRate;
            tMetaData->nWidth = videoMetaData->nWidth;
            tMetaData->nHeight = videoMetaData->nHeight;
            tMetaData->Pps = tMetaData->Sps = tMetaData->Vps = nullptr;

            _VideoMetadata.insert(std::pair<int, LPVideoMetadata >(channelID, tMetaData));

        }
    }

    if (audioMetaData)
    {
        std::lock_guard<std::mutex> lk(AudioMetaMutex);

        auto metaData = _AudioMetadata.find(channelID);
        if (metaData != _AudioMetadata.end())
        {
            if (audioMetaData->hasAudioMetaData)
            {
                metaData->second->nAudioFormat = audioMetaData->nAudioFormat;
                metaData->second->nIsStereo = audioMetaData->nIsStereo;
            }
            if (audioMetaData->hasAudioSpecCfg)
            {
                if (metaData->second->pAudioSpecCfg)
                {
                    free(metaData->second->pAudioSpecCfg);
                    metaData->second->pAudioSpecCfg = NULL;
                }

                metaData->second->nAudioSpecCfgLen = audioMetaData->nAudioSpecCfgLen;
                metaData->second->hasAudioSpecCfg = audioMetaData->hasAudioSpecCfg;
                if (metaData->second->nAudioSpecCfgLen > 0)
                {
                    metaData->second->pAudioSpecCfg = (unsigned char*)malloc(audioMetaData->nAudioSpecCfgLen);
                    memcpy(metaData->second->pAudioSpecCfg, audioMetaData->pAudioSpecCfg, audioMetaData->nAudioSpecCfgLen);
                }
            }
        }
        else
        {
            LPAudioMetadata tMetaData = (LPAudioMetadata)malloc(sizeof(AudioMetadata));
            tMetaData->pAudioSpecCfg = nullptr;
            if (audioMetaData->hasAudioMetaData)
            {
                tMetaData->hasAudioMetaData = audioMetaData->hasAudioMetaData;
                tMetaData->nAudioFormat = audioMetaData->nAudioFormat;
                tMetaData->nIsStereo = audioMetaData->nIsStereo;

            }
            if (audioMetaData->hasAudioSpecCfg)
            {
                tMetaData->hasAudioSpecCfg = audioMetaData->hasAudioSpecCfg;
                tMetaData->nAudioSpecCfgLen = audioMetaData->nAudioSpecCfgLen;

                tMetaData->pAudioSpecCfg = (unsigned char*)malloc(audioMetaData->nAudioSpecCfgLen);
                memcpy(tMetaData->pAudioSpecCfg, audioMetaData->pAudioSpecCfg, audioMetaData->nAudioSpecCfgLen);
            }

            _AudioMetadata.insert(std::pair<int, LPAudioMetadata >(channelID, tMetaData));
        }
    }

    /*
    auto metaData = _AvMetadata.find(channelID);
    if (metaData != _AvMetadata.end())
    {
        if (MetaData->hasVideoMetaData)
        {
            if (metaData->second->Sps)
            {
                free(metaData->second->Sps);
                metaData->second->Sps = NULL;
            }
            if (metaData->second->Pps)
            {
                free(metaData->second->Pps);
                metaData->second->Pps = NULL;
            }
            if (metaData->second->Vps)
            {
                free(metaData->second->Vps);
                metaData->second->Vps = NULL;
            }

            metaData->second->nWidth = MetaData->nWidth;
            metaData->second->nHeight = MetaData->nHeight;
            metaData->second->nFrameRate = MetaData->nFrameRate;

            metaData->second->nSpsLen = MetaData->nSpsLen;
            if (metaData->second->nSpsLen > 0)
            {
                metaData->second->Sps = (unsigned char*)malloc(MetaData->nSpsLen + 1);
                memcpy(metaData->second->Sps, MetaData->Sps, MetaData->nSpsLen);
            }

            metaData->second->nPpsLen = MetaData->nPpsLen;
            if (metaData->second->nPpsLen > 0)
            {
                metaData->second->Pps = (unsigned char*)malloc(MetaData->nPpsLen + 1);
                memcpy(metaData->second->Pps, MetaData->Pps, MetaData->nPpsLen);
            }

            metaData->second->nVpsLen = MetaData->nVpsLen;
            if (metaData->second->nVpsLen > 0)
            {
                metaData->second->Vps = (unsigned char*)malloc(MetaData->nVpsLen + 1);
                memcpy(metaData->second->Vps, MetaData->Vps, MetaData->nVpsLen);
            }
        }
        if (MetaData->hasAudioMetaData)
        {
            if (metaData->second->pAudioSpecCfg)
            {
                free(metaData->second->pAudioSpecCfg);
                metaData->second->pAudioSpecCfg = NULL;
            }

            metaData->second->nAudioSpecCfgLen = MetaData->nAudioSpecCfgLen;
            if (MetaData->nAudioSpecCfgLen > 0)
            {
                metaData->second->pAudioSpecCfg = (unsigned char*)malloc(MetaData->nAudioSpecCfgLen + 1);
                memcpy(metaData->second->pAudioSpecCfg, MetaData->pAudioSpecCfg, MetaData->nAudioSpecCfgLen);
            }

            metaData->second->nSamplerateIndex = MetaData->nSamplerateIndex;
            metaData->second->nIsStereo = MetaData->nIsStereo;
        }
    }
    else
    {
        LPAvMetadata newMetaData = (LPAvMetadata)malloc(sizeof(AvMetadata));
        if (MetaData->hasVideoMetaData)
        {
            newMetaData->nWidth = MetaData->nWidth;
            newMetaData->nHeight = MetaData->nHeight;
            newMetaData->nFrameRate = MetaData->nFrameRate;

            newMetaData->nSpsLen = MetaData->nSpsLen;
            if (newMetaData->nSpsLen > 0)
            {
                newMetaData->Sps = (unsigned char*)malloc(MetaData->nSpsLen + 1);
                memcpy(newMetaData->Sps, MetaData->Sps, MetaData->nSpsLen);
            }

            newMetaData->nPpsLen = MetaData->nPpsLen;
            if (newMetaData->nPpsLen > 0)
            {
                newMetaData->Pps = (unsigned char*)malloc(MetaData->nPpsLen + 1);
                memcpy(newMetaData->Pps, MetaData->Pps, MetaData->nPpsLen);
            }

            newMetaData->nVpsLen = MetaData->nVpsLen;
            if (newMetaData->nVpsLen > 0)
            {
                newMetaData->Vps = (unsigned char*)malloc(MetaData->nVpsLen + 1);
                memcpy(newMetaData->Vps, MetaData->Vps, MetaData->nVpsLen);
            }
        }

        if (MetaData->hasAudioMetaData)
        {
            newMetaData->nAudioSpecCfgLen = MetaData->nAudioSpecCfgLen;
            if (MetaData->nAudioSpecCfgLen > 0)
            {
                newMetaData->pAudioSpecCfg = (unsigned char*)malloc(MetaData->nAudioSpecCfgLen + 1);
                memcpy(newMetaData->pAudioSpecCfg, MetaData->pAudioSpecCfg, MetaData->nAudioSpecCfgLen);
            }

            newMetaData->nSamplerateIndex = MetaData->nSamplerateIndex;
            newMetaData->nIsStereo = MetaData->nIsStereo;
        }
        _AvMetadata[channelID] = newMetaData;
    }
*/
}

LPVideoMetadata IvcpData::GetVideoMetaData(int channelID)
{
    std::lock_guard<std::mutex> lk(VideoMetaMutex);

    auto metaData = _VideoMetadata.find(channelID);
    if (metaData != _VideoMetadata.end())
    {
        return CopyVideoMeta(metaData->second);
    }
    else
    {
        return nullptr;
    }
}

LPAudioMetadata IvcpData::GetAudioMetaData(int channelID)
{
    std::lock_guard<std::mutex> lk(AudioMetaMutex);
    auto metaData = _AudioMetadata.find(channelID);
    if (metaData != _AudioMetadata.end())
    {
        return CopyAudioMeta(metaData->second);
    }
    else
    {
        return nullptr;
    }
}

long long IvcpData::GetCosInfoExpireTime()
{
    std::lock_guard<std::mutex> l(SecInfoMutex);

    SecInfo.ExpireTime = SecInfo.ExpireTime - 6;
    return SecInfo.ExpireTime;
}

void IvcpData::SetCosSecInfo(ivcpGetCosSecInfoRspMsg& data)
{
    std::lock_guard<std::mutex> l(SecInfoMutex);

    SecInfo.ExpireTime = data.ExpireTime;
    SecInfo.SecretID = data.SecretID;
    SecInfo.SecretKey = data.SecretKey;
    SecInfo.SessionToken = data.SessionToken;
    SecInfo.Region = data.Region;
    SecInfo.BucketID = data.BucketID;
}

CosSecInfo IvcpData::GetCosSeCInfo()
{
    return SecInfo;
}

int IvcpData::OnImageData(const char* imageID, const unsigned char* imageData, int imageDataLen, const ivcpImageMetaData* imageMetaData)
{
    std::string url = "";
    int ret = RpcObj->OnUploadImageMetaData(imageID, imageMetaData, url);
    if (ret != Success)
    {
        LOG(INFO) << "OnImageData, OnUploadImageMetaData failed, ret :" << ret;
        return ret;
    }

    ret = CosMgr.OnImageData(imageData, imageDataLen, url);
    if (ret != Success)
    {
        LOG(INFO) << "OnImageData, UploadImage failed, ret :" << ret;
        return ret;
    }

    return ret;
}

int IvcpData::OnVideoSliceData(const char* videoSliceID, const unsigned char* videoSliceData, int videoSliceDataLen, const ivcpVideoSliceMetaData* videoSliceMetaData)
{
    std::string url = "";
    int ret = RpcObj->OnUploadVideoSliceMetaData(videoSliceID, videoSliceMetaData, url);
    if (ret != Success)
    {
        LOG(INFO) << "OnVideoSliceData, OnUploadVideoSliceMetaData failed, ret :" << ret;
        return ret;
    }

    ret = CosMgr.OnVideoSliceData(videoSliceData, videoSliceDataLen, url);
    if (ret != Success)
    {
        LOG(INFO) << "OnVideoSliceData, UploadVideoSlice failed, ret :" << ret;
        return ret;
    }

    return ret;
}

/*
int IvcpData::OnVideoSliceMetaData2(const char* videoSliceID, const ivcpVideoSliceMetaData* videoSliceMetaData,
    const ivcpVideoSliceAvParam* videoSliceAvParam)
{
    std::string url = "";
    int ret = RpcObj->OnUploadVideoSliceMetaData(videoSliceID, videoSliceMetaData, url);
    if (ret != Success)
    {
        LOG(INFO) << "OnVideoSliceMetaData2, OnUploadVideoSliceMetaData failed, ret :" << ret;
        return ret;
    }

    struct flv_metadata_t metadata;
    metadata.audiocodecid = videoSliceAvParam->audiocodecid;
    metadata.audiodatarate = videoSliceAvParam->audiodatarate;
    metadata.audiosamplerate = videoSliceAvParam->audiosamplerate;
    metadata.audiosamplesize = videoSliceAvParam->audiosamplesize;
    metadata.stereo = videoSliceAvParam->stereo;
    metadata.videocodecid = videoSliceAvParam->videocodecid;
    metadata.videodatarate = videoSliceAvParam->videodatarate;
    metadata.framerate = videoSliceAvParam->framerate;
    metadata.width = videoSliceAvParam->width;
    metadata.height = videoSliceAvParam->height;
    flv_muxer_metadata(Flv->GetFlvMuxer(), &metadata);

    return ret;
}

int IvcpData::OnVideoSliceData2(const unsigned char* data, int dataLen, int codec, unsigned int dts, unsigned int pts)
{
    // void *flv, int type, const void *data, size_t bytes, uint32_t timestamp
    return Flv->OnMuxer(data, dataLen, codec, dts, pts);
}

int IvcpData::OnFlvData(const unsigned char* videoSliceData, int videoSliceDataLen, std::string& url)
{
    int ret = CosObj->UploadVideoSliceResume(videoSliceData, videoSliceDataLen, url);
    if (ret != Success)
    {
        LOG(INFO) << "OnFlvData, UploadVideoSliceResume failed, ret :" << ret;
        return ret;
    }

    return ret;
}
*/

NatInfo IvcpData::GetNatInfo()
{
    return P2PMgr.GetNatInfo();
}

void IvcpData::DiscoverNatInfo()
{
    P2PMgr.DiscoverNatInfo();
}

void IvcpData::CloseP2P(int channelID, std::string playerID)
{
    P2PMgr.CloseP2P(channelID, playerID);
}

int IvcpData::CreateP2P(int channelID, std::string playerID, std::string playerIP, int playerPort)
{
    return P2PMgr.CreateP2P(channelID, playerID, playerIP, playerPort);
}