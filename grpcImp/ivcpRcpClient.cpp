#include <json/reader.h>
#include <json/json.h>
#include "ivcpRcpClient.h"
#include "ivcpData.h"
#include <uv.h>
#include "ivcpInternalMsgStruct.h"
#include <glog/logging.h>
#include "ivcpMsgType.h"

int IvcpRpcClient::OnCallBack(const char* requstID, int msgType, const void* msgStruct)
{
    return ClientMsgDeal->OnClientMsg(requstID, msgType, msgStruct, this);
}

int IvcpRpcClient::OnSdkInternalMsg(std::string requestID, int msgType, int status, std::string msg)
{
    return ClientMsgDeal->OnSdkInternalMsg(requestID, msgType, status, msg, this);
}

bool IvcpRpcClient::IvcpGetSvrIpMsg()
{
    ivc::RpcReqData request;
    request.set_data(IvcpRpcMsgEncoder::GetGetSvrInfoMsg());

    ivc::RpcResData reply;

    grpc::ClientContext context;

    // The actual RPC.
    grpc::Status status = Stub->IvcpGetSvrIpMsg(&context, request, &reply);

    // Act upon its status.
    if (status.ok())
    {
        LOG(INFO) << "IvcpGetSvrIpMsg success, data" << reply.data();

        ivcpGetSvrIpRspMsg data;

        if (false == IvcpRpcMsgDecoder::DecodeGetSvrIpRspMsg(reply.data(), data))
        {

            return false;
        }

        if (data.Status == 200)
        {
            IvcpData::GetInstance().SetSvrInfo(data.IvcpSvrIp);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        IvcpData::GetInstance().OnRpcFailed("IvcpGetSvrIpMsg", status);
        return false;
    }
}

bool IvcpRpcClient::IvcpRegisterMsg()
{
    // 如果已经注册了，不再注册
    if (true == IvcpData::GetInstance().GetRegisterStatus())
        return true;
    // Data we are sending to the server.
    ivc::RpcReqData request;
    request.set_data(IvcpRpcMsgEncoder::GetRegisterMsg());

    // Container for the data we expect from the server.
    ivc::RpcResData reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;

    // The actual RPC.
    grpc::Status status = Stub->IvcpRegisterMsg(&context, request, &reply);

    // Act upon its status.
    if (status.ok())
    {
        LOG(INFO) << "IvcpRegisterMsg success, data" << reply.data();

        ivcpCommonRspMsg data;

        if (false == IvcpRpcMsgDecoder::DecodeCommonRspMsg(reply.data(), data))
        {
            IvcpData::GetInstance().SetRegisterStatus(false);
            IvcpData::GetInstance().SetSvrInfo("");
            return false;
        }

        if (data.Status == 200)
        {
            IvcpData::GetInstance().SetRegisterStatus(true);
            return true;
        }
        else
        {
            IvcpData::GetInstance().SetRegisterStatus(false);
            IvcpData::GetInstance().SetSvrInfo("");
            return false;
        }
    }
    else
    {
        IvcpData::GetInstance().SetRegisterStatus(false);
        IvcpData::GetInstance().SetSvrInfo("");
        IvcpData::GetInstance().OnRpcFailed("IvcpRegisterMsg", status);
        return false;
    }
}

void IvcpRpcClient::IvcpUnRegisterMsg()
{
    ivc::RpcReqData request;
    request.set_data(IvcpRpcMsgEncoder::GetUnRegisterMsg());

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpUnRegisterMsg(&context, request, &reply);

    if (status.ok())
    {
        LOG(INFO) << "IvcpUnRegisterMsg success, data" << reply.data();
        IvcpData::GetInstance().SetRegisterStatus(false);
    }
    else
    {
        IvcpData::GetInstance().OnRpcFailed("IvcpUnRegisterMsg", status);
        return;
    }
}

bool IvcpRpcClient::IvcpKeepAliveMsg()
{
    int keepAliveCounter = IvcpData::GetInstance().GetKeepAliveCounter();
    if (keepAliveCounter > 3)
    {
        IvcpData::GetInstance().SetKeepAliveCounter(0);
        IvcpData::GetInstance().SetRegisterStatus(false);
        IvcpData::GetInstance().SetSvrInfo("");
        return false;
    }

    ivc::RpcReqData request;
    request.set_data(IvcpRpcMsgEncoder::GetKeepAliveMsg());

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpKeepAliveMsg(&context, request, &reply);

    if (status.ok())
    {
        LOG(INFO) << "IvcpKeepAliveMsg success, data" << reply.data();

        ivcpCommonRspMsg data;

        if (false == IvcpRpcMsgDecoder::DecodeCommonRspMsg(reply.data(), data))
        {
            IvcpData::GetInstance().SetKeepAliveCounter(++keepAliveCounter);
            return false;
        }

        if (data.Status == 200)
        {
            IvcpData::GetInstance().SetKeepAliveCounter(0);
            return true;
        }
        else
        {
            IvcpData::GetInstance().SetKeepAliveCounter(++keepAliveCounter);
            return false;
        }
    }
    else
    {
        IvcpData::GetInstance().SetKeepAliveCounter(++keepAliveCounter);

        IvcpData::GetInstance().OnRpcFailed("IvcpKeepAliveMsg", status);
    }
    return true;
}

bool IvcpRpcClient::IvcpGetSecretInfoMsg()
{
    if (IvcpData::GetInstance().GetCosInfoExpireTime() > 0)
        return true;

    ivc::RpcReqData request;
    request.set_data(IvcpRpcMsgEncoder::GetGetSvrInfoMsg());

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpGetNewSecMsg(&context, request, &reply);

    if (status.ok())
    {
        LOG(INFO) << "IvcpGetSecretInfoMsg success, data" << reply.data();

        ivcpGetCosSecInfoRspMsg data;

        if (false == IvcpRpcMsgDecoder::DecodeGetCosSecInfoRspMsg(reply.data(), data))
        {
            return false;
        }

        if (data.Status == 200)
        {
            IvcpData::GetInstance().SetCosSecInfo(data);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        IvcpData::GetInstance().OnRpcFailed("IvcpGetSecretInfoMsg", status);
    }
    return true;
}

void IvcpRpcClient::IvcpPollingStream()
{
    ivc::StreamReqData request;
    request.set_deviceid(IvcpData::GetInstance().GetDeviceSn());

    grpc::ClientContext context;
    if (Stream != nullptr)
    {
        Stream.reset();
    }
    Stream = Stub->IvcpPollingMsg(&context, request);

    ivc::StreamResData recv_data;
    while (Stream->Read(&recv_data))
    {
        SvrMsgDeal->OnSvrMsg(recv_data.msg_type(), recv_data.data());
    }

    LOG(INFO) << "IvcpPollingStream Read failed, try reconnect";
}

int IvcpRpcClient::OnGetDeviceBaseInfoMsg(std::string& msg)
{
    if (false == IvcpData::GetInstance().GetRegisterStatus())
        return DeviceNotRegister;

    ivc::RpcReqData request;
    request.set_data(msg);

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpRspDeviceBaseInfoMsg(&context, request, &reply);

    if (status.ok())
    {
        return Success;
    }
    IvcpData::GetInstance().OnRpcFailed("OnGetDeviceBaseInfoMsg", status);
    return RpcFailed;
}

int IvcpRpcClient::OnGetDeviceChannelInfoMsg(std::string& msg)
{
    if (false == IvcpData::GetInstance().GetRegisterStatus())
        return DeviceNotRegister;

    ivc::RpcReqData request;
    request.set_data(msg);

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpRspDeviceChannelInfoMsg(&context, request, &reply);

    if (status.ok())
    {
        return Success;
    }
    IvcpData::GetInstance().OnRpcFailed("OnGetDeviceChannelInfoMsg", status);
    return RpcFailed;
}

int IvcpRpcClient::OnGetDeviceAudioVideoInfoMsg(std::string& msg)
{
    if (false == IvcpData::GetInstance().GetRegisterStatus())
        return DeviceNotRegister;

    ivc::RpcReqData request;
    request.set_data(msg);

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpRspDeviceVideoInfoMsg(&context, request, &reply);

    if (status.ok())
    {
        return Success;
    }
    IvcpData::GetInstance().OnRpcFailed("OnGetDeviceAudioVideoInfoMsg", status);
    return RpcFailed;
}

int IvcpRpcClient::OnGetDeviceRecordInfoMsg(std::string& msg)
{
    if (false == IvcpData::GetInstance().GetRegisterStatus())
        return DeviceNotRegister;

    ivc::RpcReqData request;
    request.set_data(msg);

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpRspDeviceRecordInfoMsg(&context, request, &reply);

    if (status.ok())
    {
        return Success;
    }
    IvcpData::GetInstance().OnRpcFailed("OnGetDeviceRecordInfoMsg", status);
    return RpcFailed;
}

int IvcpRpcClient::OnModifyDeviceBaseInfoMsg(std::string& msg)
{
    if (false == IvcpData::GetInstance().GetRegisterStatus())
        return DeviceNotRegister;

    ivc::RpcReqData request;
    request.set_data(msg);

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpRspModifyDeviceBaseInfoMsg(&context, request, &reply);

    if (status.ok())
    {
        return Success;
    }
    IvcpData::GetInstance().OnRpcFailed("OnModifyDeviceBaseInfoMsg", status);
    return RpcFailed;
}

int IvcpRpcClient::OnModifyDeviceAudioVideoInfoMsg(std::string& msg)
{
    if (false == IvcpData::GetInstance().GetRegisterStatus())
        return DeviceNotRegister;

    ivc::RpcReqData request;
    request.set_data(msg);

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpRspModifyDeviceVideoInfoMsg(&context, request, &reply);

    if (status.ok())
    {
        return Success;
    }
    IvcpData::GetInstance().OnRpcFailed("OnModifyDeviceAudioVideoInfoMsg", status);
    return RpcFailed;
}

int IvcpRpcClient::OnModifyDeviceRecordInfoMsg(std::string& msg)
{
    if (false == IvcpData::GetInstance().GetRegisterStatus())
        return DeviceNotRegister;

    ivc::RpcReqData request;
    request.set_data(msg);

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpRspModifyDeviceRecordInfoMsg(&context, request, &reply);

    if (status.ok())
    {
        return Success;
    }
    IvcpData::GetInstance().OnRpcFailed("OnModifyDeviceRecordInfoMsg", status);
    return RpcFailed;
}

int IvcpRpcClient::OnGetDeviceVersionMsg(std::string& msg)
{
    if (false == IvcpData::GetInstance().GetRegisterStatus())
        return DeviceNotRegister;

    ivc::RpcReqData request;
    request.set_data(msg);

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpRspGetVersionInfoMsg(&context, request, &reply);

    if (status.ok())
    {
        return Success;
    }
    IvcpData::GetInstance().OnRpcFailed("OnGetDeviceVersionMsg", status);
    return RpcFailed;
}

int IvcpRpcClient::OnUpdateDeviceVersionMsg(std::string& msg)
{
    if (false == IvcpData::GetInstance().GetRegisterStatus())
        return DeviceNotRegister;

    ivc::RpcReqData request;
    request.set_data(msg);

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpRspUpdateVersionInfoMsg(&context, request, &reply);

    if (status.ok())
    {
        return Success;
    }
    IvcpData::GetInstance().OnRpcFailed("OnUpdateDeviceVersionMsg", status);
    return RpcFailed;
}

int IvcpRpcClient::OnPullStreamToCloudMsg(std::string& msg)
{
    if (false == IvcpData::GetInstance().GetRegisterStatus())
        return DeviceNotRegister;
    ivc::RpcReqData request;
    request.set_data(msg);

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpRspPullStreamToCloudMsg(&context, request, &reply);

    if (status.ok())
    {
        return Success;
    }
    IvcpData::GetInstance().OnRpcFailed("OnPullStreamToCloudMsg", status);
    return RpcFailed;
}

int IvcpRpcClient::IvcpUploadImageMetaDataMsg(const char* imageID, const ivcpImageMetaData* imageMetaData, std::string& url)
{
    ivc::RpcReqData request;
    request.set_data(IvcpRpcMsgEncoder::GetImageMetaDataMsg(imageID, imageMetaData));

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpUploadImageMedaDataMsg(&context, request, &reply);

    if (status.ok())
    {
        LOG(INFO) << "IvcpUploadImageMetaDataMsg success, data" << reply.data();

        ivcpUploadImageMetaDataRspMsg data;

        if (false == IvcpRpcMsgDecoder::DecodeUploadImageMetaDataRspMsg(reply.data(), data))
        {
            return JsonDecodeFailed;
        }

        if (data.Status == 200)
        {
            url = data.Url;
            return Success;
        }
        else
        {
            return UploadImageMetaDataFailed;
        }
    }
    else
    {
        IvcpData::GetInstance().OnRpcFailed("IvcpUploadImageMetaDataMsg", status);
        return RpcFailed;
    }
}

int IvcpRpcClient::IvcpUploadVideoSliceMetaDataMsg(const char* videoSliceID, const ivcpVideoSliceMetaData* videoSliceMetaData, std::string& url)
{
    ivc::RpcReqData request;
    request.set_data(IvcpRpcMsgEncoder::GetVideoSliceMetaDataMsg(videoSliceID, videoSliceMetaData));

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpUploadVideoSliceMetaDataMsg(&context, request, &reply);

    if (status.ok())
    {
        LOG(INFO) << "IvcpUploadVideoSliceMetaDataMsg success, data" << reply.data();

        ivcpUploadImageMetaDataRspMsg data;

        if (false == IvcpRpcMsgDecoder::DecodeUploadImageMetaDataRspMsg(reply.data(), data))
        {
            return JsonDecodeFailed;
        }

        if (data.Status == 200)
        {
            url = data.Url;
            return Success;
        }
        else
        {
            return UploadVideoSliceMetaDataFailed;
        }
    }
    else
    {
        IvcpData::GetInstance().OnRpcFailed("IvcpUploadVideoSliceMetaDataMsg", status);
        return RpcFailed;
    }
}

int IvcpRpcClient::IvcpReportNatInfoMsg(NatInfo info)
{
    ivc::RpcReqData request;
    request.set_data(IvcpRpcMsgEncoder::GetReportNatInfoMsg(info));

    ivc::RpcResData reply;

    grpc::ClientContext context;

    grpc::Status status = Stub->IvcpReportNatInfoMsg(&context, request, &reply);

    if (status.ok())
    {
        LOG(INFO) << "IvcpReportNatInfoMsg success, data" << reply.data();
    }
    else
    {
        IvcpData::GetInstance().OnRpcFailed("IvcpReportNatInfoMsg", status);
    }
    return true;
}