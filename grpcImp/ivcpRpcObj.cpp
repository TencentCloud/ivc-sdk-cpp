#include "ivcpRpcObj.h"
#include "ivcpRcpClient.h"
#include "ivcpData.h"

std::unique_ptr<IvcpRpcClient> IvcpRpcObj::PreClient = nullptr;
std::unique_ptr<IvcpRpcClient> IvcpRpcObj::Client = nullptr;
uv_thread_t* IvcpRpcObj::PollingHandler = nullptr;

IvcpRpcObj::IvcpRpcObj()
{
    Loop = uv_default_loop();
    RegisterHandler = new uv_timer_t;
    KeepAliveHandler = new uv_timer_t;
    GetSecInfoHandler = new uv_timer_t;
    ReportNatInfoHandler = new uv_timer_t;
    LoopHandler = new uv_thread_t;
    uv_timer_init(Loop, RegisterHandler);
    uv_timer_init(Loop, KeepAliveHandler);
    uv_timer_init(Loop, GetSecInfoHandler);
    uv_timer_init(Loop, ReportNatInfoHandler);
}

IvcpRpcObj::~IvcpRpcObj()
{
    uv_timer_stop(RegisterHandler);
    uv_timer_stop(KeepAliveHandler);
    uv_timer_stop(GetSecInfoHandler);
    uv_timer_stop(ReportNatInfoHandler);
    uv_loop_close(Loop);

    delete RegisterHandler;
    delete KeepAliveHandler;
    delete GetSecInfoHandler;
    delete ReportNatInfoHandler;
    delete LoopHandler;
}

void IvcpRpcObj::StartTask()
{
    // uv_timer_start，参数有4个，分别是：timer定时器，回调函数，延时时间，重复间隔。
    uv_timer_start(RegisterHandler, RegisterCallBack, 5000, 5000);
    uv_timer_start(KeepAliveHandler, KeepAliveCallBack, 20000, 10000);
    uv_timer_start(GetSecInfoHandler, GetSecInfoCallBack, 1000, 10000);
    uv_timer_start(ReportNatInfoHandler, ReportNatInfoCallBack, 1000, 10000);

    uv_thread_create(LoopHandler, LoopThreadFunc, Loop);
}

int IvcpRpcObj::OnCallBack(const char* requstID, int msgType, const void* msgStruct)
{
    return Client->OnCallBack(requstID, msgType, msgStruct);
}

void IvcpRpcObj::OnSdkInternalMsg(std::string requestID, int msgType, int status, std::string msg)
{
    Client->OnSdkInternalMsg(requestID, msgType, status, msg);
}

void IvcpRpcObj::LoopThreadFunc(void* arg)
{
    uv_loop_t* loop = static_cast<uv_loop_t*>(arg);
    uv_run(loop, UV_RUN_DEFAULT);
}

void IvcpRpcObj::PollingThreadFunc(void* arg)
{
    bool status = IvcpData::GetInstance().GetRegisterStatus();
    if (status)
    {
        Client->IvcpPollingStream();
        if (PollingHandler != nullptr)
        {
            delete PollingHandler;
            PollingHandler = nullptr;
        }
    }
}

void IvcpRpcObj::KeepAliveCallBack(uv_timer_t* handle)
{
    // 判断是否注册成功，如果成功，进行保活
    // 如果没成功，则跳过
    bool status = IvcpData::GetInstance().GetRegisterStatus();
    if (status)
    {
        Client->IvcpKeepAliveMsg();
    }
}

void IvcpRpcObj::GetSecInfoCallBack(uv_timer_t* handle)
{
    // 判断是否注册成功，如果成功，进行保活
    // 如果没成功，则跳过
    bool status = IvcpData::GetInstance().GetRegisterStatus();
    if (status)
    {
        Client->IvcpGetSecretInfoMsg();
    }
}

void IvcpRpcObj::ReportNatInfoCallBack(uv_timer_t* handle)
{
    bool status = IvcpData::GetInstance().GetRegisterStatus();
    if (status)
    {
        auto info = IvcpData::GetInstance().GetNatInfo();

        IvcpData::GetInstance().DiscoverNatInfo();

        Client->IvcpReportNatInfoMsg(info);
    }
}

void IvcpRpcObj::RegisterCallBack(uv_timer_t* handle)
{
    bool ret = IvcpData::GetInstance().PeekSvrInfo();
    if (ret == false)
    {
        if (PreClient == nullptr)
        {
            std::string target = IvcpData::GetInstance().GetIpByName("ivcp.ivc.myqcloud.com");
            target = target + ":" + IvcpData::GetInstance().GetSvrPort();

            std::unique_ptr<IvcpRpcClient> ptr(new IvcpRpcClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials())));
            PreClient = std::move(ptr);
        }
        PreClient->IvcpGetSvrIpMsg();
        return;
    }

    PreClient.release();

    if (Client == nullptr)
    {
        std::string target = IvcpData::GetInstance().GetSvrIp() + ":" + IvcpData::GetInstance().GetSvrPort();
        std::unique_ptr<IvcpRpcClient> ptr(new IvcpRpcClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials())));
        Client = std::move(ptr);
    }

    ret = Client->IvcpRegisterMsg();
    if (ret == true)
    {
        if (PollingHandler)
            return;
        PollingHandler = new uv_thread_t;
        uv_thread_create(PollingHandler, PollingThreadFunc, nullptr);
        return;
    }

    Client.release();
}

int IvcpRpcObj::OnUploadImageMetaData(const char* imageID, const ivcpImageMetaData* imageMetaData, std::string& url)
{
    bool status = IvcpData::GetInstance().GetRegisterStatus();
    if (status)
    {
        return Client->IvcpUploadImageMetaDataMsg(imageID, imageMetaData, url);
    }
}

int IvcpRpcObj::OnUploadVideoSliceMetaData(const char* videoSliceID, const ivcpVideoSliceMetaData* videoSliceMetaData, std::string& url)
{
    bool status = IvcpData::GetInstance().GetRegisterStatus();
    if (status)
    {
        return Client->IvcpUploadVideoSliceMetaDataMsg(videoSliceID, videoSliceMetaData, url);
    }
}