#include "ivcpMsgType.h"
#include "ivcpMsgStruct.h"
#include <string.h>
#include <stdlib.h>

static void FreeAudioMeta(LPAudioMetadata data)
{
    if (data)
    {
        if (data->pAudioSpecCfg)
            free(data->pAudioSpecCfg);
        data->pAudioSpecCfg = nullptr;
    }
    data = nullptr;
}

static LPAudioMetadata CopyAudioMeta(LPAudioMetadata audioMetaData)
{
    if (audioMetaData)
    {
        LPAudioMetadata tMetaData = (LPAudioMetadata)malloc(sizeof(AudioMetadata));
        tMetaData->pAudioSpecCfg = nullptr;
        if (audioMetaData->hasAudioMetaData)
        {
            tMetaData->hasAudioMetaData = audioMetaData->hasAudioMetaData;
            tMetaData->nAudioFormat = audioMetaData->nAudioFormat;
            tMetaData->nIsStereo = audioMetaData->nIsStereo;

        }
        if (audioMetaData->hasAudioSpecCfg)
        {
            tMetaData->hasAudioSpecCfg = audioMetaData->hasAudioSpecCfg;
            tMetaData->nAudioSpecCfgLen = audioMetaData->nAudioSpecCfgLen;

            tMetaData->pAudioSpecCfg = (unsigned char*)malloc(audioMetaData->nAudioSpecCfgLen);
            memcpy(tMetaData->pAudioSpecCfg, audioMetaData->pAudioSpecCfg, audioMetaData->nAudioSpecCfgLen);
        }

        return tMetaData;
    }
    return nullptr;
}

static LPVideoMetadata CopyVideoMeta(LPVideoMetadata videoMetaData)
{
    if (videoMetaData)
    {
        LPVideoMetadata tMetaData = (LPVideoMetadata)malloc(sizeof(VideoMetadata));
        tMetaData->nFrameRate = videoMetaData->nFrameRate;
        tMetaData->nWidth = videoMetaData->nWidth;
        tMetaData->nHeight = videoMetaData->nHeight;
        tMetaData->Pps = tMetaData->Sps = tMetaData->Vps = nullptr;

        if (videoMetaData->Sps)
        {
            tMetaData->Sps = (unsigned char*)malloc(videoMetaData->nSpsLen);
            memcpy(tMetaData->Sps, videoMetaData->Sps, videoMetaData->nSpsLen);
            tMetaData->nSpsLen = videoMetaData->nSpsLen;
        }

        if (videoMetaData->Pps)
        {
            tMetaData->Pps = (unsigned char*)malloc(videoMetaData->nPpsLen);
            memcpy(tMetaData->Pps, videoMetaData->Pps, videoMetaData->nPpsLen);
            tMetaData->nPpsLen = videoMetaData->nPpsLen;
        }

        if (videoMetaData->Vps)
        {
            tMetaData->Vps = (unsigned char*)malloc(videoMetaData->nVpsLen);
            memcpy(tMetaData->Vps, videoMetaData->Vps, videoMetaData->nVpsLen);
            tMetaData->nVpsLen = videoMetaData->nVpsLen;
        }
        return tMetaData;
    }
    return nullptr;
}

static void FreeVideoMeta(LPVideoMetadata data)
{
    if (data)
    {
        if (data->Pps)
            free(data->Pps);
        data->Pps = nullptr;

        if (data->Sps)
            free(data->Sps);
        data->Sps = nullptr;

        if (data->Vps)
            free(data->Vps);
        data->Vps = nullptr;
    }
    data = nullptr;
}
