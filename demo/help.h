#include <cstring>
#include <stdio.h>
#include "mpeg-proto.h"
#include "flv-demuxer.h"
#include "opus-head.h"
#include "mpeg4-aac.h"
#include "aom-av1.h"
#include "mpeg4-avc.h"
#include "mpeg4-hevc.h"
#include "mpeg4-vvc.h"
#include "ivcpMsgStruct.h"
#include "ivcpInterface.h"
#include "flv-proto.h"
#include "ivcpMsgType.h"

enum AMF
{
    //8byte Double
    AMF_DATA_TYPE_NUMBER = 0x00,
    //bool
    AMF_DATA_TYPE_BOOL = 0x01,
    //后面2个字节为长度
    AMF_DATA_TYPE_STRING = 0x02,
    AMF_DATA_TYPE_OBJECT = 0x03,
    AMF_DATA_TYPE_NULL = 0x05,
    AMF_DATA_TYPE_UNDEFINED = 0x06,
    AMF_DATA_TYPE_REFERENCE = 0x07,
    //数组 类似map
    AMF_DATA_TYPE_MIXEDARRAY = 0x08,
    AMF_DATA_TYPE_OBJECT_END = 0x09,
    AMF_DATA_TYPE_ARRAY = 0x0A,
    AMF_DATA_TYPE_DATE = 0x0B,
    //后面4个字节为长度
    AMF_DATA_TYPE_LONG_STRING = 0x0C,
    AMF_DATA_TYPE_UNSUPPORTED = 0x0D
};

unsigned int char2Int16(unsigned char ch[])
{
    unsigned int result = 0;
    result = ch[0];
    result <<= 8;
    result |= ch[1];
    return result;
}

unsigned int char2Int24(unsigned char ch[])
{
    unsigned int result = 0;
    result = ch[0];
    result <<= 8;
    result |= ch[1];
    result <<= 8;
    result |= ch[2];
    return result;
}

unsigned int char2Int32(unsigned char ch[])
{
    unsigned int result = 0;
    result = ch[0];
    result <<= 8;
    result |= ch[1];
    result <<= 8;
    result |= ch[2];
    result <<= 8;
    result |= ch[3];
    return result;
}

unsigned long char2Int64(unsigned char ch[])
{
    unsigned long result = 0;
    result = ch[0];
    result <<= 8;
    result |= ch[1];
    result <<= 8;
    result |= ch[2];
    result <<= 8;
    result |= ch[3];
    result <<= 8;
    result |= ch[4];
    result <<= 8;
    result |= ch[5];
    result <<= 8;
    result |= ch[6];
    result <<= 8;
    result |= ch[7];
    return result;
}

void reverseCharArray8(unsigned char ch[])
{
    for (int i = 0; i < 4; i++)
    {
        char temp = ch[i];
        ch[i] = ch[7 - i];
        ch[7 - i] = temp;
    }
}

void readFLVScriptData(char* data, int datalen, VideoMetadata& vMetadata, AudioMetadata& aMetaData)
{
    char type = 0;
    unsigned char strLength[2] = { 0 };
    int cur = 0;
    type = data[cur];
    cur = cur + 1;
    memcpy(strLength, &data[cur], 2);
    if (type == 0x02)
    {
        //printf("First AFM TYPE: String\n");
    }
    cur = cur + 2;
    const int stringLength = char2Int16(strLength);
    //printf("First AFM String Length: %d\n", stringLength);

    auto stringData = new char[stringLength];
    memcpy(stringData, &data[cur], stringLength);
    cur = cur + stringLength;
    //printf("First AFM String Data: %s\n", stringData);
    delete[]stringData;

    char type2 = 0;
    memcpy(&type2, &data[cur], 1);
    cur = cur + 1;
    if (type2 == 0x08)
    {
        //printf("Second AFM TYPE: Array\n");
    }

    unsigned char arrayNums[4] = { 0 };
    memcpy(&arrayNums, &data[cur], 4);
    cur = cur + 4;
    int nums = char2Int32(arrayNums);
    //printf("Second AFM Array Nums: %d\n", nums);

    for (int i = 0; i < nums; i++)
    {
        unsigned char length[2] = { 0 };
        //iStream.read((char*)length, 2);
        memcpy(&length, &data[cur], 2);
        cur = cur + 2;
        int llength = char2Int16(length);
        auto str = new char[llength];
        memset(str, 0, llength);
        //iStream.read(str, llength);
        memcpy(str, &data[cur], llength);
        cur = cur + llength;

        char metaType = 0;
        // iStream.read(&metaType, 1);
        memcpy(&metaType, &data[cur], 1);
        cur = cur + 1;
        switch (metaType)
        {
        case AMF_DATA_TYPE_NUMBER:
        {
            unsigned char num[8] = { 0 };
            // iStream.read((char*)num, 8);
            memcpy(&num, &data[cur], 8);
            cur = cur + 8;
            reverseCharArray8(num);
            //printf("Second AFM Data Key: %s Value: %lf\n", str, *(double*)num);
            if (std::string(str) == std::string("width"))
            {
                vMetadata.nWidth = *(double*)num;
            }
            else if (std::string(str) == std::string("height"))
            {
                vMetadata.nHeight = *(double*)num;
            }
            else if (std::string(str) == std::string("frameraterate"))
            {
                vMetadata.nFrameRate = *(double*)num;
            }
            else if (std::string(str) == std::string("audiocodecidize"))
            {
                aMetaData.nAudioFormat = *(double*)num;
            }
            else if (std::string(str) == std::string("audiosamplerate"))
            {
                aMetaData.nSamplerate = *(double*)num;
            }
            break;
        }
        case AMF_DATA_TYPE_BOOL:
        {
            char data1 = 0;
            // iStream.read(&data, 1);
            memcpy(&data1, &data[cur], 1);
            cur = cur + 1;
            // printf("Second AFM Data Key: %s Value: %d\n", str, data1);
            if (std::string(str) == std::string("stereo"))
            {
                aMetaData.nIsStereo = data1;
            }
            break;
        }
        case AMF_DATA_TYPE_STRING:
        {
            unsigned char len[2] = { 0 };
            //iStream.read((char*)len, 2);
            memcpy(&len, &data[cur], 2);
            cur = cur + 2;
            int m_len = char2Int16(len);
            char* data1 = new char[m_len];
            //iStream.read(data1, m_len);
            memcpy(data1, &data[cur], m_len);
            cur = cur + m_len;
            // printf("Second AFM Data Key: %s Value: %s\n", str, data1);
            delete[]data1;
            break;
        }
        case AMF_DATA_TYPE_LONG_STRING:
        {
            unsigned char len[4] = { 0 };
            // iStream.read((char*)len, 4);
            memcpy(&len, &data[cur], 4);
            cur = cur + 4;
            int m_len = char2Int32(len);
            char* data1 = new char[m_len];
            //iStream.read(data1, m_len);
            memcpy(data1, &data[cur], m_len);
            cur = cur + m_len;
            // printf("Second AFM Data Key: %s Value: %s\n", str, data1);
            delete[]data1;
            break;
        }
        }
        delete[]str;
    }
    unsigned char end[3] = { 0 };
    //iStream.read((char*)end, 3);
    memcpy(&end, &data[cur], 3);
    cur = cur + 3;
    int num = char2Int24(end);
    // printf("Second AFM END: %d\n", num);
}

struct my_flv_demuxer_t
{
    union
    {
        struct mpeg4_aac_t aac;
        struct opus_head_t opus;
    } a;

    union
    {
        struct aom_av1_t av1;
        struct mpeg4_avc_t avc;
        struct mpeg4_hevc_t hevc;
        struct mpeg4_vvc_t vvc;
    } v;

    flv_demuxer_handler handler;
    void* param;

    uint8_t* ptr;
    int capacity;
};

static int flv_handler(void* param, int codec, const void* data, size_t bytes, uint32_t pts, uint32_t dts, int flags)
{
    switch (codec)
    {
    case FLV_VIDEO_AVCC: //264 seq
    {
        my_flv_demuxer_t* demuxer = (my_flv_demuxer_t*)param;
        {
            int spsLen = demuxer->v.avc.sps[0].bytes + 4;

            unsigned char* sps = new unsigned char[spsLen];
            sps[0] = 0;
            sps[1] = 0;
            sps[2] = 0;
            sps[3] = 1;
            memcpy(&sps[4], demuxer->v.avc.sps[0].data, demuxer->v.avc.sps[0].bytes);

            IvcpAudioVideoData(0, 1, (const unsigned char*)sps, spsLen, H264, 0, 0);
            delete[] sps;
        }
        {
            int ppsLen = demuxer->v.avc.pps[0].bytes + 4;

            unsigned char* pps = new unsigned char[ppsLen];
            pps[0] = 0;
            pps[1] = 0;
            pps[2] = 0;
            pps[3] = 1;
            memcpy(&pps[4], demuxer->v.avc.pps[0].data, demuxer->v.avc.pps[0].bytes);

            IvcpAudioVideoData(0, 1, (const unsigned char*)pps, ppsLen, H264, 0, 0);
            delete[]pps;
        }
        return Success;
    }
    case FLV_AUDIO_ASC://aac cfg
    {
        AudioMetadata metadata;
        metadata.hasAudioSpecCfg = 1;
        metadata.hasAudioMetaData = 0;
        metadata.nAudioSpecCfgLen = bytes;
        metadata.pAudioSpecCfg = (unsigned char*)data;

        return IvcpAudioVideoData(1, 1, nullptr, 0, 0, 0, 0, nullptr, &metadata);
    }
    case FLV_VIDEO_H264:
    {
        return IvcpAudioVideoData(0, 1, (const unsigned char*)data, bytes, H264, dts, pts);
    }

    case FLV_AUDIO_AAC: //aac数据
        return IvcpAudioVideoData(0, 1, (const unsigned char*)data, bytes, AAC, dts, pts);
    default:
        return Success;
    }
}