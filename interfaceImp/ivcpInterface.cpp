#include "ivcpInterface.h"
#include <vector>
#include <glog/logging.h>
#include "ivcpData.h"
#include "ivcpMsgType.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int IvcpInitIvcpSdk(const char* deviceSN, void (*p)(const void*, int))
    {
        /*
        设置日志
        注册回调
        初始化单例类
        */
        google::InitGoogleLogging("ivcp_sdk");
        google::EnableLogCleaner(1);
        FLAGS_log_dir = "./";
        FLAGS_alsologtostderr = false;
        FLAGS_max_log_size = 50;
        FLAGS_logbufsecs = 0;

        if (deviceSN == NULL)
        {
            LOG(INFO) << "deviceSN is null";
            return IllegalParameter;
        }
        if (p == NULL)
        {
            LOG(INFO) << "p is null";
            return IllegalParameter;
        }
        IvcpData::GetInstance().RegisterCallback(p);

        IvcpData::GetInstance().IvcpDataInit(deviceSN);

        return Success;
    }

    int IvcpUnInitIvcpSdk()
    {
        return Success;
    }

    int IvcpUploadImageData(const char* imageID, const unsigned char* imageData, int imageDataLen, const ivcpImageMetaData* imageMetaData)
    {
        if (imageID == NULL || imageData == NULL || imageMetaData == NULL || imageDataLen <= 0)
        {
            return IllegalParameter;
        }

        return IvcpData::GetInstance().OnImageData(imageID, imageData, imageDataLen, imageMetaData);
    }

    int IvcpUploadVideoSliceData(const char* videoSliceID, const unsigned char* videoSliceData, int videoSliceDataLen, const ivcpVideoSliceMetaData* videoSliceMetaData)
    {
        if (videoSliceID == NULL || videoSliceData == NULL || videoSliceMetaData == NULL || videoSliceDataLen <= 0)
        {
            return IllegalParameter;
        }

        return IvcpData::GetInstance().OnVideoSliceData(videoSliceID, videoSliceData, videoSliceDataLen, videoSliceMetaData);
    }

    /*
        int IvcpUploadVideoSliceMetaData2(const char *videoSliceID, const ivcpVideoSliceMetaData *videoSliceMetaData, const ivcpVideoSliceAvParam *videoSliceAvParam)
        {
            if (videoSliceID == NULL || 6 == NULL || videoSliceAvParam == NULL)
            {
                return IllegalParameter;
            }

            return IvcpData::GetInstance().OnVideoSliceMetaData2(videoSliceID, videoSliceMetaData, videoSliceAvParam);
        }

        int IvcpUploadVideoSliceData2(const unsigned char *data, int dataLen, int codec, unsigned int dts, unsigned int pts)
        {
            if (data == NULL || dataLen <= 0)
            {
                return IllegalParameter;
            }

            return IvcpData::GetInstance().OnVideoSliceData2(data, dataLen, codec, dts, pts);
        }
    */

    int IvcpAudioVideoData(int isMetaData, int channelID, const unsigned char* data, int dataLen,
        int dataType, unsigned int dts, unsigned int pts,
        const LPVideoMetadata videoMetaData, const LPAudioMetadata audioMetaData)
    {
        if (isMetaData)
        {
            if (videoMetaData == NULL && audioMetaData == NULL)
            {
                return IllegalParameter;
            }
            else
            {
                IvcpData::GetInstance().OnAvMetaData(channelID, videoMetaData, audioMetaData);
                return Success;
            }
        }
        else
        {

            if (data == NULL || dataLen <= 0 || dts < 0 || pts < 0)
            {
                return IllegalParameter;
            }

            IvcpData::GetInstance().OnAvData(channelID, data, dataLen, dataType, dts, pts);
            return Success;
        }
    }

    int IvcpCommonMsg(const char* requstID, int msgType, const void* msgStruct)
    {
        if (requstID == NULL || msgStruct == NULL)
        {
            return IllegalParameter;
        }
        return IvcpData::GetInstance().OnCallBack(requstID, msgType, msgStruct);
    }
#ifdef __cplusplus
};
#endif
