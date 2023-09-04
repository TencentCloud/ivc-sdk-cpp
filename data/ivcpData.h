#ifndef _IVCP_IVCPDATA_H__
#define _IVCP_IVCPDATA_H__

#include <string>
#include "ivcpRcpClient.h"
#include "ivcpRpcObj.h"
#include <mutex>
#include <librtmp/amf.h>
#include <librtmp/rtmp.h>
#include <librtmp/rtmp_sys.h>
#include "ivcpCos.h"
#include "ivcpAvData.h"
#include "ivcpCosInfo.h"
#include "ivcpRtmpMgr.h"
#include "ivcpCosMgr.h"
#include "ivcpP2PMgr.h"

class IvcpData
{
public:
    static IvcpData& GetInstance()
    {
        static IvcpData instance;
        return instance;
    }
    ~IvcpData();

private:
    IvcpData();
    IvcpData(IvcpData const&) = delete;
    IvcpData& operator=(const IvcpData&) = delete;

public:
    int IvcpDataInit(const char* deviceSN);
    void RegisterCallback(void (*p)(const void*, int));

public:
    //SDK 收流接口的数据统一到这里
    void OnAvData(int channelID, const unsigned char* data, int dataLen, int dataType, unsigned int dts, unsigned int pts);
    void OnAvMetaData(int channelID, const LPVideoMetadata videoMetaData, const LPAudioMetadata audioMetaData);

public:
    void OnVideoMetaData(int channelID, const LPVideoMetadata videoMetaData);

public:
    LPVideoMetadata GetVideoMetaData(int channelID);
    LPAudioMetadata GetAudioMetaData(int channelID);

public:
    void OnRpcFailed(std::string caller, grpc::Status& status);
    std::string GetDeviceSn();
    std::string GetIpByName(std::string);
    std::string GetSvrPort();

    // 注册登录相关
    bool GetRegisterStatus();
    void SetRegisterStatus(bool);
    int GetKeepAliveCounter();
    void SetKeepAliveCounter(int);
    bool PeekSvrInfo();
    void SetSvrInfo(std::string data);
    std::string GetSvrIp();

    // rtmp相关
    void CloseRtmp(int channelID);
    bool CreateRtmp(int channelID, std::string url);

    // cos 相关
    long long GetCosInfoExpireTime();
    void SetCosSecInfo(ivcpGetCosSecInfoRspMsg& data);
    int OnImageData(const char* imageID, const unsigned char* imageData, int imageDataLen, const ivcpImageMetaData* imageMetaData);
    int OnVideoSliceData(const char* videoSliceID, const unsigned char* videoSliceData, int videoSliceDataLen, const ivcpVideoSliceMetaData* videoSliceMetaData);
    // int OnVideoSliceMetaData2(const char* videoSliceID, const ivcpVideoSliceMetaData* videoSliceMetaData, const ivcpVideoSliceAvParam* videoSliceAvParam);
    // int OnVideoSliceData2(const unsigned char* data, int dataLen, int codec, unsigned int dts, unsigned int pts);
    // int OnFlvData(const unsigned char* videoSliceData, int videoSliceDataLen, std::string& url);
    bool CreateWriteCos(int channelid, std::string streamid, std::string planid);
    void CloseWriteCos(int channelid);
    CosSecInfo GetCosSeCInfo();

    // P2P相关
    NatInfo GetNatInfo();
    void DiscoverNatInfo();
    void CloseP2P(int channelID, std::string playerID);
    int CreateP2P(int channelID, std::string playerID, std::string playerIP, int playerPort);

public:
    // 服务器消息 --->  客户端
    int MsgToClient(void* msg, int msgType);
    // 客户端消息 --->  服务器
    int OnCallBack(const char* requstID, int msgType, const void* msgStruct);
    // sdk消息 --->  服务器
    void OnSdkInternalMsg(std::string requestID, int msgType, int status, std::string msg);

private:
    void Test();
private:
    std::string SvrPort;
    std::string SvrDomainName;
    std::string SvrIp;

private:
    // rpc实例
    IvcpRpcObj* RpcObj;
    // 设备ID
    std::string DeviceSn;
    // 服务器消息 --->  客户端的回调函数指针
    void (*Callback)(const void*, int);

    // 注册状态
    bool RegisterStatus;
    std::mutex RegisterMutex;
    // 保活计数
    int KeepAliveCounter;

    // cos 密钥
    std::mutex SecInfoMutex;
    CosSecInfo SecInfo;
    int ExpireTime;

    // cosMgr
    IvcpCosMgr CosMgr;

    // rtmpMgr
    IvcpRtmpMgr RtmpMgr;

    // p2pMgr
    IvcpP2PMgr P2PMgr;

    // 音视频元数据
    std::map<int, LPVideoMetadata>      _VideoMetadata;
    std::mutex VideoMetaMutex;

    std::map<int, LPAudioMetadata>  _AudioMetadata;
    std::mutex AudioMetaMutex;
};

#endif
