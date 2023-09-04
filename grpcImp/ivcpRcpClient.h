#ifndef _IVCP_IVCPRPCCLIENT_H__
#define _IVCP_IVCPRPCCLIENT_H__

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "ivcp.grpc.pb.h"
#include "ivcpRpcMsgEncode.h"
#include "ivcpOnSvrMsg.h"
#include "ivcpOnClientMsg.h"
#include "ivcpRpcMsgDecode.h"
#include "Discovery.h"

class IvcpRpcClient
{

public:
  // IvcpRpcClient();

  IvcpRpcClient(std::shared_ptr<grpc::Channel> channel) : Stub(ivc::ivcp::NewStub(channel)),
    Stream(nullptr),
    SvrMsgDeal(std::make_unique<IvcpOnSvrMsg>()),
    ClientMsgDeal(std::make_unique<IvcpOnClientMsg>())
  {
  }

public:
  int OnCallBack(const char* requstID, int msgType, const void* msgStruct);
  int OnSdkInternalMsg(std::string requestID, int msgType, int status, std::string msg);

public: // 需要独立线程处理
  void IvcpPollingStream();

public: // 设备主动
  bool IvcpGetSvrIpMsg();

  bool IvcpRegisterMsg();

  void IvcpUnRegisterMsg();

  bool IvcpKeepAliveMsg();

  bool IvcpGetSecretInfoMsg();

  int IvcpUploadImageMetaDataMsg(const char* imageID, const ivcpImageMetaData* imageMetaData, std::string& url);

  int IvcpUploadVideoSliceMetaDataMsg(const char* videoSliceID, const ivcpVideoSliceMetaData* videoSliceMetaData, std::string& url);

  int IvcpReportNatInfoMsg(NatInfo info);

public: // 设备被动
  int OnGetDeviceBaseInfoMsg(std::string& msg);

  int OnGetDeviceChannelInfoMsg(std::string& msg);

  int OnGetDeviceAudioVideoInfoMsg(std::string& msg);

  int OnGetDeviceRecordInfoMsg(std::string& msg);

  int OnModifyDeviceBaseInfoMsg(std::string& msg);

  int OnModifyDeviceAudioVideoInfoMsg(std::string& msg);

  int OnModifyDeviceRecordInfoMsg(std::string& msg);

  int OnGetDeviceVersionMsg(std::string& msg);

  int OnUpdateDeviceVersionMsg(std::string& msg);

  int OnPullStreamToCloudMsg(std::string& msg);

private:
  std::unique_ptr<ivc::ivcp::Stub> Stub;
  std::unique_ptr<grpc::ClientReader<ivc::StreamResData>> Stream;
  std::unique_ptr<IvcpOnSvrMsg> SvrMsgDeal;
  std::unique_ptr<IvcpOnClientMsg> ClientMsgDeal;
};

#endif