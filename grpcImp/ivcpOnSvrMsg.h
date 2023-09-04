#ifndef _IVCP_IVCPONSVRMSG_H__
#define _IVCP_IVCPONSVRMSG_H__

#include <string>

class IvcpOnSvrMsg
{
public:
    void OnSvrMsg(int msgType, std::string msg);

private:
    void OnGetDeviceBaseMsg(int msgType, std::string& msg);
    void OnGetDeviceChannelMsg(int msgType, std::string& msg);
    void OnGetDeviceAudioVideoMsg(int msgType, std::string& msg);
    void OnGetDeviceRecordMsg(int msgType, std::string& msg);

    void OnModifyDeviceBaseMsg(int msgType, std::string& msg);
    void OnModifyDeviceAudioVideoMsg(int msgType, std::string& msg);
    void OnModifyDeviceecordMsg(int msgType, std::string& msg);

    void OnPullStreamToCloudMsg(int msgType, std::string& msg);
    void OnPullStreamToCosMsg(int msgType, std::string& msg);
    void OnPullStreamToP2PMsg(int msgType, std::string& msg);
};

#endif