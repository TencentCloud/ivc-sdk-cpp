#include "ivcpOnClientMsg.h"
#include "ivcpMsgStruct.h"
#include "ivcpMsgType.h"
#include <string.h>
#include <json/reader.h>
#include <json/json.h>
#include "ivcpData.h"
#include "ivcpRpcMsgEncode.h"
#include "ivcpRcpClient.h"

int IvcpOnClientMsg::OnClientMsg(const char *requstID, int msgType, const void *msgStruct, IvcpRpcClient *cli)
{
    int ret = Success;
    switch (msgType)
    {
    case IvcpRequsetDeviceBaseInfo:
        ret = OnDeviceBaseMsg(requstID, msgStruct, cli);
        break;
    case IvcpRequsetDeviceChannelInfo:
        ret = OnDeviceChannelMsg(requstID, msgStruct, cli);
        break;
    case IvcpRequsetDeviceAudioVideoInfo:
        ret = OnDeviceAudioVideoMsg(requstID, msgStruct, cli);
        break;
    case IvcpRequsetDeviceRecordInfo:
        ret = OnDeviceRecordMsg(requstID, msgStruct, cli);
        break;
    case IvcpModifyDeviceBaseInfo:
        ret = OnModifyDeviceBaseInfoMsg(requstID, msgStruct, cli);
        break;
    case IvcpModifyDeviceAudioVideoInfo:
        ret = OnModifyDeviceAudioVideoInfoMsg(requstID, msgStruct, cli);
        break;
    case IvcpModifyDeviceRecordInfo:
        ret = OnModifyDeviceRecordInfoMsg(requstID, msgStruct, cli);
        break;
    case IvcpRequsetPullStreamToCloud:
        ret = OnPullStreamToCloudMsg(requstID, msgStruct, cli);
        break;
    case IvcpRequsetGetDeviceVersionInfo:
        ret = OnGetDeviceVersionMsg(requstID, msgStruct, cli);
        break;
    case IvcpRequsetUpdateDeviceVersionInfo:
        ret = OnUpdateDeviceVersionMsg(requstID, msgStruct, cli);
        break;

    default:
        break;
    }

    return ret;
}

int IvcpOnClientMsg::OnSdkInternalMsg(std::string requestID, int msgType, int status, std::string msg, IvcpRpcClient *cli)
{
    auto rpcMsg = IvcpRpcMsgEncoder::GetCommonRspMsg(requestID, status, msg);
    int ret = Success;
    switch (msgType)
    {
    case IvcpRequsetPullStreamToCloud:
        ret = cli->OnPullStreamToCloudMsg(rpcMsg);
        break;

    default:
        break;
    }
    return ret;
}

int IvcpOnClientMsg::OnDeviceBaseMsg(const char *requstID, const void *msg, IvcpRpcClient *cli)
{
    const ivcpGetDeviceBaseRspInfo *structMsg = static_cast<const ivcpGetDeviceBaseRspInfo *>(msg);

    auto rpcMsg = IvcpRpcMsgEncoder::GetDeviceBaseInfoMsg(std::string(requstID), structMsg);

    return cli->OnGetDeviceBaseInfoMsg(rpcMsg);
}

int IvcpOnClientMsg::OnDeviceChannelMsg(const char *requstID, const void *msg, IvcpRpcClient *cli)
{
    const ivcpGetDeviceChannelRspInfo *structMsg = static_cast<const ivcpGetDeviceChannelRspInfo *>(msg);

    auto rpcMsg = IvcpRpcMsgEncoder::GetDeviceChannelInfoMsg(std::string(requstID), structMsg);

    return cli->OnGetDeviceChannelInfoMsg(rpcMsg);
}

int IvcpOnClientMsg::OnDeviceAudioVideoMsg(const char *requstID, const void *msg, IvcpRpcClient *cli)
{
    const ivcpGetDeviceAudioVideoRspInfo *structMsg = static_cast<const ivcpGetDeviceAudioVideoRspInfo *>(msg);

    auto rpcMsg = IvcpRpcMsgEncoder::GetDeviceAudioVideoInfoMsg(std::string(requstID), structMsg);

    return cli->OnGetDeviceAudioVideoInfoMsg(rpcMsg);
}

int IvcpOnClientMsg::OnDeviceRecordMsg(const char *requstID, const void *msg, IvcpRpcClient *cli)
{
    const ivcpGetDeviceRecordRspInfo *structMsg = static_cast<const ivcpGetDeviceRecordRspInfo *>(msg);

    auto rpcMsg = IvcpRpcMsgEncoder::GetDeviceRecordInfoMsg(std::string(requstID), structMsg);

    return cli->OnGetDeviceRecordInfoMsg(rpcMsg);
}

int IvcpOnClientMsg::OnPullStreamToCloudMsg(const char *requstID, const void *msg, IvcpRpcClient *cli)
{
    const ivcpPullStreamToCloudRspMsg *structMsg = static_cast<const ivcpPullStreamToCloudRspMsg *>(msg);

    auto rpcMsg = IvcpRpcMsgEncoder::GetCommonRspMsg(requstID, structMsg->code, structMsg->msg);

    return cli->OnPullStreamToCloudMsg(rpcMsg);
}

int IvcpOnClientMsg::OnModifyDeviceBaseInfoMsg(const char *requstID, const void *msg, IvcpRpcClient *cli)
{
    const ivcpModifyDeviceBaseRspInfo *structMsg = static_cast<const ivcpModifyDeviceBaseRspInfo *>(msg);

    auto rpcMsg = IvcpRpcMsgEncoder::GetModifyDeviceBaseInfoMsg(std::string(requstID), structMsg);

    return cli->OnModifyDeviceBaseInfoMsg(rpcMsg);
}

int IvcpOnClientMsg::OnModifyDeviceAudioVideoInfoMsg(const char *requstID, const void *msg, IvcpRpcClient *cli)
{
    const ivcpModifyDeviceAudioVideoRspInfo *structMsg = static_cast<const ivcpModifyDeviceAudioVideoRspInfo *>(msg);

    auto rpcMsg = IvcpRpcMsgEncoder::GetModifyDeviceAudioVideoInfoMsg(std::string(requstID), structMsg);

    return cli->OnModifyDeviceAudioVideoInfoMsg(rpcMsg);
}

int IvcpOnClientMsg::OnModifyDeviceRecordInfoMsg(const char *requstID, const void *msg, IvcpRpcClient *cli)
{
    const ivcpModifyDeviceRecordRspInfo *structMsg = static_cast<const ivcpModifyDeviceRecordRspInfo *>(msg);

    auto rpcMsg = IvcpRpcMsgEncoder::GetModifyDeviceRecordInfoMsg(std::string(requstID), structMsg);

    return cli->OnModifyDeviceRecordInfoMsg(rpcMsg);
}

int IvcpOnClientMsg::OnGetDeviceVersionMsg(const char *requstID, const void *msg, IvcpRpcClient *cli)
{
    const ivcpGetDeviceVersionRspInfo *structMsg = static_cast<const ivcpGetDeviceVersionRspInfo *>(msg);

    auto rpcMsg = IvcpRpcMsgEncoder::GetDeviceVersionMsg(std::string(requstID), structMsg);

    return cli->OnGetDeviceVersionMsg(rpcMsg);
}

int IvcpOnClientMsg::OnUpdateDeviceVersionMsg(const char *requstID, const void *msg, IvcpRpcClient *cli)
{
    const ivcpUpdateDeviceVersionRspInfo *structMsg = static_cast<const ivcpUpdateDeviceVersionRspInfo *>(msg);

    auto rpcMsg = IvcpRpcMsgEncoder::GetUpdateDeviceVersionMsg(std::string(requstID), structMsg);

    return cli->OnUpdateDeviceVersionMsg(rpcMsg);
}