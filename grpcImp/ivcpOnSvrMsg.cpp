#include "ivcpOnSvrMsg.h"
#include "ivcpMsgStruct.h"
#include "ivcpMsgType.h"
#include <string.h>
#include <json/reader.h>
#include <json/json.h>
#include "ivcpData.h"
#include <glog/logging.h>

void IvcpOnSvrMsg::OnSvrMsg(int msgType, std::string msg)
{

    switch (msgType)
    {
    case IvcpRequsetDeviceBaseInfo:
        OnGetDeviceBaseMsg(msgType, msg);
        break;
    case IvcpRequsetDeviceChannelInfo:
        OnGetDeviceChannelMsg(msgType, msg);
        break;
    case IvcpRequsetDeviceAudioVideoInfo:
        OnGetDeviceAudioVideoMsg(msgType, msg);
        break;
    case IvcpRequsetDeviceRecordInfo:
        OnGetDeviceRecordMsg(msgType, msg);
        break;
    case IvcpModifyDeviceBaseInfo:
        OnModifyDeviceBaseMsg(msgType, msg);
        break;
    case IvcpModifyDeviceAudioVideoInfo:
        OnModifyDeviceAudioVideoMsg(msgType, msg);
        break;
    case IvcpModifyDeviceRecordInfo:
        OnModifyDeviceecordMsg(msgType, msg);
        break;
    case IvcpRequsetPullStreamToCloud:
        OnPullStreamToCloudMsg(msgType, msg);
        break;
    case IvcpRequsetPullStreamToCos:
        OnPullStreamToCosMsg(msgType, msg);
        break;
    case IvcpRequsetPullStreamToP2PInfo:
        OnPullStreamToP2PMsg(msgType, msg);
        break;
    default:
        break;
    }
}

void IvcpOnSvrMsg::OnGetDeviceBaseMsg(int msgType, std::string& msg)
{
    Json::Value root;

    std::string caller = "OnGetDeviceBaseMsg";
    if (!IvcpRpcMsgDecoder::DecodeJson(msg, root, caller))
    {
        return;
    }

    std::string deviceID = root["device_id"].asString();
    std::string requestID = root["request_id"].asString();

    auto info = std::make_shared<ivcpGetDeviceBaseInfo>();
    info->deviceID = deviceID.c_str();
    info->requestID = requestID.c_str();

    IvcpData::GetInstance().MsgToClient(info.get(), msgType);
}

void IvcpOnSvrMsg::OnGetDeviceChannelMsg(int msgType, std::string& msg)
{
    Json::Value root;

    std::string caller = "OnGetDeviceChannelMsg";
    if (!IvcpRpcMsgDecoder::DecodeJson(msg, root, caller))
    {
        return;
    }

    std::string deviceID = root["device_id"].asString();
    std::string requestID = root["request_id"].asString();

    auto info = std::make_shared<ivcpGetDeviceChannelInfo>();
    info->deviceID = deviceID.c_str();
    info->requestID = requestID.c_str();

    IvcpData::GetInstance().MsgToClient(info.get(), msgType);
}

void IvcpOnSvrMsg::OnGetDeviceAudioVideoMsg(int msgType, std::string& msg)
{
    Json::Value root;

    std::string caller = "OnGetDeviceAudioVideoMsg";
    if (!IvcpRpcMsgDecoder::DecodeJson(msg, root, caller))
    {
        return;
    }

    std::string deviceID = root["device_id"].asString();
    std::string requestID = root["request_id"].asString();

    auto info = std::make_shared<ivcpGetDeviceBaseInfo>();
    info->deviceID = deviceID.c_str();
    info->requestID = requestID.c_str();

    IvcpData::GetInstance().MsgToClient(info.get(), msgType);
}

void IvcpOnSvrMsg::OnGetDeviceRecordMsg(int msgType, std::string& msg)
{
    Json::Value root;

    std::string caller = "OnGetDeviceRecordMsg";
    if (!IvcpRpcMsgDecoder::DecodeJson(msg, root, caller))
    {
        return;
    }

    std::string deviceID = root["device_id"].asString();
    std::string requestID = root["request_id"].asString();

    auto info = std::make_shared<ivcpGetDeviceBaseInfo>();
    info->deviceID = deviceID.c_str();
    info->requestID = requestID.c_str();

    IvcpData::GetInstance().MsgToClient(info.get(), msgType);
}

void IvcpOnSvrMsg::OnModifyDeviceBaseMsg(int msgType, std::string& msg)
{
    Json::Value root;

    std::string caller = "OnModifyDeviceBaseMsg";
    if (!IvcpRpcMsgDecoder::DecodeJson(msg, root, caller))
    {
        return;
    }

    std::string deviceID = root["device_id"].asString();
    std::string requestID = root["request_id"].asString();

    Json::Value data = root["data"];

    std::string DeviceName = data["device_name"].asString();

    auto info = std::make_shared<ivcpModifyDeviceBaseInfo>();
    info->deviceID = deviceID.c_str();
    info->requestID = requestID.c_str();
    info->DeviceName = DeviceName.c_str();

    IvcpData::GetInstance().MsgToClient(info.get(), msgType);
}

void IvcpOnSvrMsg::OnModifyDeviceAudioVideoMsg(int msgType, std::string& msg)
{
    Json::Value root;

    std::string caller = "OnModifyDeviceAudioVideoMsg";
    if (!IvcpRpcMsgDecoder::DecodeJson(msg, root, caller))
    {
        return;
    }

    std::string deviceID = root["device_id"].asString();
    std::string requestID = root["request_id"].asString();

    Json::Value data = root["data"];
    Json::Value resolution = data["resolution"];

    std::string VideoCodecType = root["video_codec_type"].asString();
    std::string AudioCodecType = root["audio_codec_type	"].asString();
    std::string StreamType = root["stream_type"].asString();
    std::string ComplexType = root["complex_type"].asString();
    std::string CodeType = root["code_type"].asString();
    int CodeRate = data["code_rate"].asInt();
    int Height = resolution["height"].asInt();
    int Width = resolution["width"].asInt();
    int Fps = data["fps"].asInt();
    int Quality = data["quality"].asInt();
    int Gop = data["gop"].asInt();

    auto info = std::make_shared<ivcpModifyDeviceAudioVideoInfo>();
    info->deviceID = deviceID.c_str();
    info->requestID = requestID.c_str();
    info->VideoCodecType = VideoCodecType.c_str();
    info->AudioCodecType = AudioCodecType.c_str();
    info->StreamType = StreamType.c_str();
    info->ComplexType = ComplexType.c_str();
    info->CodeType = CodeType.c_str();
    info->CodeRate = CodeRate;
    info->Height = Height;
    info->Width = Width;
    info->Fps = Fps;
    info->Quality = Quality;
    info->Gop = Gop;

    IvcpData::GetInstance().MsgToClient(info.get(), msgType);
}

void IvcpOnSvrMsg::OnModifyDeviceecordMsg(int msgType, std::string& msg)
{
    Json::Value root;

    std::string caller = "OnModifyDeviceecordMsg";
    if (!IvcpRpcMsgDecoder::DecodeJson(msg, root, caller))
    {
        return;
    }

    std::string deviceID = root["device_id"].asString();
    std::string requestID = root["request_id"].asString();

    Json::Value data = root["data"];

    int ImageNum = data["image_num"].asInt();
    int VideoDuration = data["video_duration"].asInt();
    int ImageOnly = data["image_only"].asInt();

    auto info = std::make_shared<ivcpModifyDeviceRecordInfo>();
    info->deviceID = deviceID.c_str();
    info->requestID = requestID.c_str();
    info->ImageNum = ImageNum;
    info->VideoDuration = VideoDuration;
    info->ImageOnly = ImageOnly;

    IvcpData::GetInstance().MsgToClient(info.get(), msgType);
}

void IvcpOnSvrMsg::OnPullStreamToCloudMsg(int msgType, std::string& msg)
{
    Json::Value root;

    std::string caller = "OnPullStreamToCloudMsg";
    if (!IvcpRpcMsgDecoder::DecodeJson(msg, root, caller))
    {
        return;
    }

    std::string deviceID = root["device_id"].asString();
    std::string requestID = root["request_id"].asString();
    std::string rtmpUrl = root["rtmp_url"].asString();
    std::string action = root["action"].asString();
    int channelid = root["channel_id"].asInt();

    auto info = std::make_shared<ivcpPullStreamToCloudMsg>();
    info->deviceID = deviceID.c_str();
    info->requestID = requestID.c_str();

    if (action.compare("close") == 0)
    {
        IvcpData::GetInstance().CloseRtmp(channelid);
        IvcpData::GetInstance().OnSdkInternalMsg(requestID, msgType, Success, "");
        return;
    }

    IvcpData::GetInstance().CloseRtmp(channelid);

    //  创建新的rtmp推流对象并初始化
    bool ret = IvcpData::GetInstance().CreateRtmp(channelid, rtmpUrl);
    if (false == ret)
    {
        IvcpData::GetInstance().OnSdkInternalMsg(requestID, msgType, ConnectRtmpSvrFailed, "Connect rtmp svr failed");
        return;
    }
    IvcpData::GetInstance().OnSdkInternalMsg(requestID, msgType, Success, "");
}

void IvcpOnSvrMsg::OnPullStreamToCosMsg(int msgType, std::string& msg)
{
    Json::Value root;

    std::string caller = "OnPullStreamToCosMsg";
    if (!IvcpRpcMsgDecoder::DecodeJson(msg, root, caller))
    {
        return;
    }

    std::string deviceID = root["device_id"].asString();
    std::string requestID = root["request_id"].asString();
    std::string StreamId = root["stream_id"].asString();
    std::string action = root["action"].asString();
    std::string PlanID = root["plan_id"].asString();
    int channelid = root["channel_id"].asInt();

    // 如果是关，则关闭写cos
    if (action.compare("close") == 0)
    {
        IvcpData::GetInstance().CloseWriteCos(channelid);
        IvcpData::GetInstance().OnSdkInternalMsg(requestID, msgType, Success, "");
        return;
    }

    //  创建写cos对象，如果对象存在，则直接返回true

    bool ret = IvcpData::GetInstance().CreateWriteCos(channelid, StreamId, PlanID);
    if (false == ret)
    {
        IvcpData::GetInstance().OnSdkInternalMsg(requestID, msgType, WriteCosFailed, "write cos failed");
        return;
    }
    IvcpData::GetInstance().OnSdkInternalMsg(requestID, msgType, Success, "");
}

void IvcpOnSvrMsg::OnPullStreamToP2PMsg(int msgType, std::string& msg)
{
    Json::Value root;

    std::string caller = "OnPullStreamToP2PMsg";
    if (!IvcpRpcMsgDecoder::DecodeJson(msg, root, caller))
    {
        return;
    }

    std::string deviceID = root["device_id"].asString();
    std::string requestID = root["request_id"].asString();
    std::string playerID = root["player_id"].asString();
    int channelID = root["channel_id"].asInt();
    std::string action = root["action"].asString();
    std::string playerIP = root["PlayerIp"].asString();
    int playerPort = root["PlayerPort"].asInt();


    // 如果是关，则关闭P2p
    if (action.compare("close") == 0)
    {
        IvcpData::GetInstance().CloseP2P(channelID, playerID);
        IvcpData::GetInstance().OnSdkInternalMsg(requestID, msgType, Success, "");
        return;
    }

    //  创建P2P
    int ret = IvcpData::GetInstance().CreateP2P(channelID, playerID, playerIP, playerPort);
    if (false == ret)
    {
        IvcpData::GetInstance().OnSdkInternalMsg(requestID, msgType, WriteCosFailed, "write cos failed");
        return;
    }
    IvcpData::GetInstance().OnSdkInternalMsg(requestID, msgType, Success, "");
}

