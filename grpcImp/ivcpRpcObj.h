#ifndef _IVCP_IvcpRpcObj_H__
#define _IVCP_IvcpRpcObj_H__

#include <uv.h>
#include "ivcpRcpClient.h"

class IvcpRpcObj
{
public:
    IvcpRpcObj();
    ~IvcpRpcObj();

public:
    void StartTask();

    int OnCallBack(const char *requstID, int msgType, const void *msgStruct);

    void OnSdkInternalMsg(std::string requestID, int msgType, int status, std::string msg);

public:
    int OnUploadImageMetaData(const char *imageID, const ivcpImageMetaData *imageMetaData, std::string &url);
    int OnUploadVideoSliceMetaData(const char *videoSliceID, const ivcpVideoSliceMetaData *videoSliceMetaData, std::string &url);

protected:
    static void RegisterCallBack(uv_timer_t *handle);

    static void KeepAliveCallBack(uv_timer_t *handle);

    static void GetSecInfoCallBack(uv_timer_t *handle);

    static void ReportNatInfoCallBack(uv_timer_t *handle);

    static void PollingThreadFunc(void *arg);

    static void LoopThreadFunc(void *arg);

protected:
    static std::unique_ptr<IvcpRpcClient> PreClient;
    static std::unique_ptr<IvcpRpcClient> Client;
    static uv_thread_t *PollingHandler;

private:
    uv_loop_t *Loop;
    uv_timer_t *RegisterHandler;
    uv_timer_t *KeepAliveHandler;
    uv_timer_t *GetSecInfoHandler;
    uv_timer_t *ReportNatInfoHandler;

    uv_thread_t *LoopHandler;
    bool ThreadFlag;
};

#endif