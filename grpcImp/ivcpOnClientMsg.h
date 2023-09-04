#ifndef _IVCP_IVCPONCLIENTMSG_H__
#define _IVCP_IVCPONCLIENTMSG_H__

#include <string>
class IvcpRpcClient;

class IvcpOnClientMsg
{
public:
    int OnClientMsg(const char *requstID, int msgType, const void *msgStruct, IvcpRpcClient *cli);

    int OnSdkInternalMsg(std::string requestID, int msgType, int status, std::string msg, IvcpRpcClient *cli);

private:
    int OnDeviceBaseMsg(const char *requstID, const void *msg, IvcpRpcClient *cli);

    int OnDeviceChannelMsg(const char *requstID, const void *msg, IvcpRpcClient *cli);

    int OnDeviceAudioVideoMsg(const char *requstID, const void *msg, IvcpRpcClient *cli);

    int OnDeviceRecordMsg(const char *requstID, const void *msg, IvcpRpcClient *cli);

    int OnModifyDeviceBaseInfoMsg(const char *requstID, const void *msg, IvcpRpcClient *cli);

    int OnModifyDeviceAudioVideoInfoMsg(const char *requstID, const void *msg, IvcpRpcClient *cli);

    int OnModifyDeviceRecordInfoMsg(const char *requstID, const void *msg, IvcpRpcClient *cli);

    int OnGetDeviceVersionMsg(const char *requstID, const void *msg, IvcpRpcClient *cli);

    int OnUpdateDeviceVersionMsg(const char *requstID, const void *msg, IvcpRpcClient *cli);

    int OnPullStreamToCloudMsg(const char *requstID, const void *msg, IvcpRpcClient *cli);
};

#endif