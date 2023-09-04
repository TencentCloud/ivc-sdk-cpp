
#ifndef _IVCP_MSGTYPE_H__
#define _IVCP_MSGTYPE_H__

enum AVType {
    H264 = 1,
    H265 = 2,
    AAC = 3,
    G711U = 4,
    G711A = 5,
};
enum IvcpMsgType
{
    IvcpRequsetDeviceBaseInfo = 1000,
    IvcpRequsetDeviceChannelInfo,
    IvcpRequsetDeviceAudioVideoInfo,
    IvcpRequsetDeviceRecordInfo,
    IvcpModifyDeviceBaseInfo,
    IvcpModifyDeviceAudioVideoInfo,
    IvcpModifyDeviceRecordInfo,
    IvcpRequsetPullStreamToCloud,       // 服务器请求视频流上云直播
    IvcpRequsetPullStreamToP2PInfo,     // 服务器请求视频流上P2P
    IvcpRequsetPullStreamToCos,         // 服务器请求视频流存cos
    IvcpRequsetGetDeviceVersionInfo,    // 服务器获取设备版本信息
    IvcpRequsetUpdateDeviceVersionInfo, // 服务器请求设备升级版本
};

enum IvcpErrorCode
{
    Success = 0,

    DeviceNotRegister = 400,
    DeviceNotExists = 403,
    DuplicateDeviceID = 409,
    RequsetNatsFailed = 409,
    JsonDecodeFailed = 418, // 后面的状态码不需要定义值，往下累加就行
    IllegalParameter,
    InsertDBFailed,
    CMSRequestParamError,
    DataConvertFailed,
    DuplicateRequestId,
    WaitDeviceResponseTimeout,
    ImageIdNotExists,
    WriteCosFailed,
    DBSearchFailed,
    RegisterLock,
    SendStreamFailed,
    ConnectRtmpSvrFailed,
    GetSecInfoFailed,
    UploadImageMetaDataFailed,
    UploadVideoSliceMetaDataFailed,
    RpcFailed,
    CreateTheadFailed,
    CheckBucketFailed,
    BucketNotExist,
    CosClientError,
    P2PSessiocExist,
};
#endif