#include <json/reader.h>
#include <json/json.h>
#include "ivcpRpcMsgEncode.h"
#include "ivcpData.h"

std::string IvcpRpcMsgEncoder::JsonEncoder(Json::Value& root)
{
    std::string str;
    Json::StreamWriterBuilder writerBuilder;
    std::ostringstream os;
    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(root, &os);
    str = os.str();

    return str;
}

std::string IvcpRpcMsgEncoder::GetCommonMsg()
{
    Json::Value root;

    root["DeviceID"] = IvcpData::GetInstance().GetDeviceSn();

    return JsonEncoder(root);
}

std::string IvcpRpcMsgEncoder::GetGetSvrInfoMsg()
{
    return GetCommonMsg();
}

std::string IvcpRpcMsgEncoder::GetRegisterMsg()
{
    return GetCommonMsg();
}

std::string IvcpRpcMsgEncoder::GetUnRegisterMsg()
{
    return GetCommonMsg();
}

std::string IvcpRpcMsgEncoder::GetKeepAliveMsg()
{
    return GetCommonMsg();
}

std::string IvcpRpcMsgEncoder::GetDeviceBaseInfoMsg(std::string requstID, const ivcpGetDeviceBaseRspInfo* sMsg)
{
    Json::Value root;

    root["code"] = sMsg->code;
    root["msg"] = sMsg->msg;
    root["request_id"] = std::string(requstID);
    root["device_id"] = IvcpData::GetInstance().GetDeviceSn();
    Json::Value data;
    data["device_model"] = sMsg->DeviceModel;
    data["manufacturer"] = sMsg->Manufacturer;
    data["firmware_version"] = sMsg->FirmwareVersion;
    data["device_name"] = sMsg->DeviceName;
    data["protocol_type"] = sMsg->ProtocolType;

    root["data"] = data;

    return JsonEncoder(root);

}

std::string IvcpRpcMsgEncoder::GetDeviceChannelInfoMsg(std::string requstID, const ivcpGetDeviceChannelRspInfo* sMsg)
{
    Json::Value root;

    root["code"] = sMsg->code;
    root["msg"] = sMsg->msg;
    root["request_id"] = std::string(requstID);
    root["device_id"] = IvcpData::GetInstance().GetDeviceSn();
    root["channel_num"] = sMsg->channelNum;
    Json::Value data;
    Json::Value array;
    for (int i = 0; i < sMsg->channelNum; i++)
    {
        Json::Value item;
        item["channel_id"] = sMsg->channelInfos[i].channelID;
        item["channel_name"] = sMsg->channelInfos[i].channelName;
        item["channel_type"] = sMsg->channelInfos[i].channelType;
        item["channel_status"] = sMsg->channelInfos[i].channelState;

        array.append(item);
    }
    data["data"] = array;
    root["data"] = data;

    return JsonEncoder(root);
}

std::string IvcpRpcMsgEncoder::GetCommonRspMsg(std::string requstID, int status, std::string msg)
{
    Json::Value root;

    root["code"] = status == 0 ? 200 : status;
    root["msg"] = msg;
    root["request_id"] = requstID;
    root["device_id"] = IvcpData::GetInstance().GetDeviceSn();
    Json::Value data;
    root["data"] = data;

    return JsonEncoder(root);
}

std::string IvcpRpcMsgEncoder::GetImageMetaDataMsg(const char* imageID, const ivcpImageMetaData* imageMetaData)
{
    Json::Value root;

    root["DeviceID"] = imageMetaData->DeviceID;
    root["ChannelID"] = imageMetaData->ChannelID;
    root["ID"] = std::string(imageID);
    root["FileFormate"] = imageMetaData->FileFormat;
    root["ShotTime"] = (Json::Value::Int64)(imageMetaData->ShotTime);
    root["Title"] = imageMetaData->Title;
    root["Height"] = imageMetaData->Height;
    root["Width"] = imageMetaData->Width;
    root["FileSize"] = imageMetaData->FileSize;
    root["EventType"] = imageMetaData->EventType;

    return JsonEncoder(root);
}

std::string IvcpRpcMsgEncoder::GetVideoSliceMetaDataMsg(const char* videoSliceID, const ivcpVideoSliceMetaData* videoSliceMetaData)
{
    Json::Value root;

    root["DeviceID"] = videoSliceMetaData->DeviceID;
    root["ChannelID"] = videoSliceMetaData->ChannelID;
    root["ID"] = std::string(videoSliceID);
    root["FileFormate"] = videoSliceMetaData->FileFormat;
    root["BeginTime"] = (Json::Value::Int64)(videoSliceMetaData->BeginTime);
    root["EndTime"] = (Json::Value::Int64)(videoSliceMetaData->EndTime);
    root["Title"] = videoSliceMetaData->Title;
    root["Height"] = videoSliceMetaData->Height;
    root["Width"] = videoSliceMetaData->Width;
    root["FileSize"] = videoSliceMetaData->FileSize;
    root["EventType"] = videoSliceMetaData->EventType;

    return JsonEncoder(root);
}

std::string IvcpRpcMsgEncoder::GetDeviceAudioVideoInfoMsg(std::string requstID, const ivcpGetDeviceAudioVideoRspInfo* sMsg)
{
    Json::Value root;

    root["code"] = sMsg->code;
    root["msg"] = sMsg->msg;
    root["request_id"] = std::string(requstID);
    root["device_id"] = IvcpData::GetInstance().GetDeviceSn();

    Json::Value array;

    for (size_t i = 1; i <= sMsg->channelNum; i++)
    {
        Json::Value data;
        data["channel_id"] = i;
        data["video_codec_type"] = sMsg->AvInfos[i].VideoCodecType;
        data["audio_codec_type"] = sMsg->AvInfos[i].AudioCodecType;
        data["stream_type"] = sMsg->AvInfos[i].StreamType;
        data["complex_type"] = sMsg->AvInfos[i].ComplexType;
        data["code_rate"] = sMsg->AvInfos[i].CodeRate;
        data["code_type"] = sMsg->AvInfos[i].CodeType;
        data["fps"] = sMsg->AvInfos[i].Fps;
        data["quality"] = sMsg->AvInfos[i].Quality;
        data["gop"] = sMsg->AvInfos[i].Gop;
        Json::Value resolution;
        resolution["height"] = sMsg->AvInfos[i].Height;
        resolution["width"] = sMsg->AvInfos[i].Width;
        data["resolution"] = resolution;

        array.append(data);
    }

    Json::Value data;
    data["infos"] = array;
    root["data"] = data;

    return JsonEncoder(root);
}

std::string IvcpRpcMsgEncoder::GetDeviceRecordInfoMsg(std::string requstID, const ivcpGetDeviceRecordRspInfo* sMsg)
{
    Json::Value root;

    root["code"] = sMsg->code;
    root["msg"] = sMsg->msg;
    root["request_id"] = std::string(requstID);
    root["device_id"] = IvcpData::GetInstance().GetDeviceSn();

    Json::Value array;

    for (size_t i = 1; i <= sMsg->channelNum; i++)
    {
        Json::Value data;
        data["channel_id"] = i;
        data["image_num"] = sMsg->RecordInfos[i].ImageNum;
        data["video_duration"] = sMsg->RecordInfos[i].VideoDuration;
        data["image_only"] = sMsg->RecordInfos[i].ImageOnly;
    }

    Json::Value data;
    data["infos"] = array;
    root["data"] = data;

    return JsonEncoder(root);
}

std::string IvcpRpcMsgEncoder::GetModifyDeviceBaseInfoMsg(std::string requstID, const ivcpModifyDeviceBaseRspInfo* sMsg)
{
    return GetCommonRspMsg(requstID, sMsg->code, sMsg->msg);
}

std::string IvcpRpcMsgEncoder::GetModifyDeviceAudioVideoInfoMsg(std::string requstID, const ivcpModifyDeviceAudioVideoRspInfo* sMsg)
{
    return GetCommonRspMsg(requstID, sMsg->code, sMsg->msg);
}

std::string IvcpRpcMsgEncoder::GetModifyDeviceRecordInfoMsg(std::string requstID, const ivcpModifyDeviceRecordRspInfo* sMsg)
{
    return GetCommonRspMsg(requstID, sMsg->code, sMsg->msg);
}

std::string IvcpRpcMsgEncoder::GetDeviceVersionMsg(std::string requstID, const ivcpGetDeviceVersionRspInfo* sMsg)
{
    Json::Value root;


    root["code"] = sMsg->code;
    root["msg"] = sMsg->msg;
    root["request_id"] = std::string(requstID);
    root["device_id"] = IvcpData::GetInstance().GetDeviceSn();
    Json::Value data;
    data["current_version"] = sMsg->CurrentFirmwareVersion;
    data["new_version"] = sMsg->NewFirmwareVersion;

    root["data"] = data;

    return JsonEncoder(root);

}

std::string IvcpRpcMsgEncoder::GetUpdateDeviceVersionMsg(std::string requstID, const ivcpUpdateDeviceVersionRspInfo* sMsg)
{
    return GetCommonRspMsg(requstID, sMsg->code, sMsg->msg);
}

std::string IvcpRpcMsgEncoder::GetReportNatInfoMsg(NatInfo info)
{
    Json::Value root;

    root["DeviceID"] = IvcpData::GetInstance().GetDeviceSn();
    root["NatType"] = info.NatType;
    root["Ip"] = info.NatIp;
    root["Port"] = info.NatPort;
    root["NatName"] = info.NatName;

    return JsonEncoder(root);
}
