#ifndef _IVCP_IVCPINTERNALMSGSTRUCT_H__
#define _IVCP_IVCPINTERNALMSGSTRUCT_H__

#include <string>

// 设备请求服务器后，服务器返回的结果
struct ivcpCommonRspMsg
{
    std::string DeviceID = "";
    int Status = 0;
    std::string Msg = "";
};

// 设备请求获取服务器地址后，服务器返回的结果
struct ivcpGetSvrIpRspMsg
{
    std::string DeviceID = "";
    int Status = 0;
    std::string Msg = "";
    std::string IvcpSvrIp = "";
};

// 设备请求获取cos临时密钥后，服务器的返回信息
struct ivcpGetCosSecInfoRspMsg
{
    std::string DeviceID = "";
    int Status = 0;
    std::string Msg = "";
    std::string SecretID = "";
    std::string SecretKey = "";
    std::string SessionToken = "";
    long long ExpireTime = 0;
    std::string Region = "";
    std::string BucketID = "";
};

// 设备上传图片相信信息时，服务器返回的结果
struct ivcpUploadImageMetaDataRspMsg
{
    std::string DeviceID = "";
    int Status = 0;
    std::string Msg = "";
    std::string Url = "";
};
#endif