#ifndef _IVCP_IVCPRPMSGDECODE_H__
#define _IVCP_IVCPRPMSGDECODE_H__

#include <string>
#include "ivcpMsgStruct.h"
#include <json/reader.h>
#include <json/json.h>
#include "ivcpInternalMsgStruct.h"

class IvcpRpcMsgDecoder
{
public:
    static bool DecodeCommonRspMsg(std::string msg, ivcpCommonRspMsg& root);

    static bool DecodeGetSvrIpRspMsg(std::string msg, ivcpGetSvrIpRspMsg& data);

    static bool DecodeGetCosSecInfoRspMsg(std::string msg, ivcpGetCosSecInfoRspMsg& data);

    static bool DecodeUploadImageMetaDataRspMsg(std::string msg, ivcpUploadImageMetaDataRspMsg& data);

public:
    static bool DecodeJson(std::string msg, Json::Value& root, std::string& caller);
};

#endif