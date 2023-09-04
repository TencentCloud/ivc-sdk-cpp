#include "ivcpRpcMsgDecode.h"
#include <glog/logging.h>

bool IvcpRpcMsgDecoder::DecodeJson(std::string msg, Json::Value& root, std::string& caller)
{
    std::string err;
    Json::CharReaderBuilder read_builder;

    std::unique_ptr<Json::CharReader> const jsonReader(read_builder.newCharReader());
    bool res = jsonReader->parse(msg.c_str(), msg.c_str() + msg.length(), &root, &err);
    // 如果服务器的消息解析不了，直接丢弃
    if (!res || !err.empty())
    {

        LOG(INFO) << caller << ", parseJson error, msg: " << msg << " err: " << err;
        return false;
    }
    return true;
}

bool IvcpRpcMsgDecoder::DecodeCommonRspMsg(std::string msg, ivcpCommonRspMsg& data)
{
    Json::Value root;

    std::string caller = "DecodeCommonRspMsg";
    if (!DecodeJson(msg, root, caller))
    {
        return false;
    }

    data.DeviceID = root["DeviceID"].asString();
    data.Status = root["Status"].asInt();
    data.Msg = root["Msg"].asString();

    return true;
}

bool IvcpRpcMsgDecoder::DecodeGetSvrIpRspMsg(std::string msg, ivcpGetSvrIpRspMsg& data)
{
    Json::Value root;

    std::string caller = "DecodeGetSvrIpRspMsg";
    if (!DecodeJson(msg, root, caller))
    {
        return false;
    }

    data.DeviceID = root["DeviceID"].asString();
    data.Status = root["Status"].asInt();
    data.Msg = root["Msg"].asString();
    data.IvcpSvrIp = root["IvcpSvrIp"].asString();

    return true;
}

bool IvcpRpcMsgDecoder::DecodeGetCosSecInfoRspMsg(std::string msg, ivcpGetCosSecInfoRspMsg& data)
{
    Json::Value root;

    std::string caller = "DecodeGetCosSecInfoRspMsg";
    if (!DecodeJson(msg, root, caller))
    {
        return false;
    }
    data.DeviceID = root["DeviceID"].asString();
    data.Status = root["Status"].asInt();
    data.Msg = root["Msg"].asString();
    data.SecretID = root["SecretId"].asString();
    data.SecretKey = root["SecretKey"].asString();
    data.SessionToken = root["SessionToken"].asString();
    data.ExpireTime = root["ExpireTime"].asInt64();
    data.Region = root["Region"].asString();
    data.BucketID = root["BucketId"].asString();

    return true;
}

bool IvcpRpcMsgDecoder::DecodeUploadImageMetaDataRspMsg(std::string msg, ivcpUploadImageMetaDataRspMsg& data)
{
    Json::Value root;

    std::string caller = "DecodeUploadImageMetaDataRspMsg";
    if (!DecodeJson(msg, root, caller))
    {
        return false;
    }

    data.DeviceID = root["DeviceID"].asString();
    data.Status = root["Status"].asInt();
    data.Msg = root["Msg"].asString();
    data.Url = root["Url"].asString();

    return true;
}

