#ifndef _IVCP_IVCPRPMSGENCODE_H__
#define _IVCP_IVCPRPMSGENCODE_H__

#include <string>
#include "ivcpMsgStruct.h"
#include "Discovery.h"
#include <json/json.h>

class IvcpRpcMsgEncoder
{
public:
  static std::string GetGetSvrInfoMsg();

  static  std::string GetRegisterMsg();

  static std::string GetUnRegisterMsg();

  static std::string GetKeepAliveMsg();

  static std::string GetImageMetaDataMsg(const char* imageID, const ivcpImageMetaData* imageMetaData);

  static std::string GetVideoSliceMetaDataMsg(const char* videoSliceID, const ivcpVideoSliceMetaData* videoSliceMetaData);

  static std::string GetReportNatInfoMsg(NatInfo info);

public:
  static std::string GetDeviceBaseInfoMsg(std::string requstID, const ivcpGetDeviceBaseRspInfo* sMsg);

  static std::string GetDeviceChannelInfoMsg(std::string requstID, const ivcpGetDeviceChannelRspInfo* sMsg);

  static std::string GetDeviceAudioVideoInfoMsg(std::string requstID, const ivcpGetDeviceAudioVideoRspInfo* sMsg);

  static std::string GetDeviceRecordInfoMsg(std::string requstID, const ivcpGetDeviceRecordRspInfo* sMsg);

  static std::string GetModifyDeviceBaseInfoMsg(std::string requstID, const ivcpModifyDeviceBaseRspInfo* sMsg);

  static std::string GetModifyDeviceAudioVideoInfoMsg(std::string requstID, const ivcpModifyDeviceAudioVideoRspInfo* sMsg);

  static std::string GetModifyDeviceRecordInfoMsg(std::string requstID, const ivcpModifyDeviceRecordRspInfo* sMsg);

  static std::string GetDeviceVersionMsg(std::string requstID, const ivcpGetDeviceVersionRspInfo* sMsg);

  static std::string GetUpdateDeviceVersionMsg(std::string requstID, const ivcpUpdateDeviceVersionRspInfo* sMsg);

  static std::string GetCommonRspMsg(std::string requstID, int status, std::string msg);

private:
  static std::string GetCommonMsg();

  static std::string JsonEncoder(Json::Value& root);
};

#endif