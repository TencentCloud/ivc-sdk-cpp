
#ifndef _IVCP_AVDATA_H__
#define _IVCP_AVDATA_H__


#include <string>
#include <cstring>

struct AvData
{
    unsigned char* avData;
    int avDataLen;
    int avDataType;
    unsigned int avDts;
    unsigned int avPts;
    int channelID;
    bool isKeyFrame;

    AvData(int channelid, const unsigned char* data, int dataLen, int dataType, unsigned int dts, unsigned int pts, bool key) {
        avData = new unsigned char[dataLen];
        memcpy(avData, data, dataLen);
        avDataLen = dataLen;
        avDataType = dataType;
        avDts = dts;
        avPts = pts;
        channelID = channelid;
        isKeyFrame = key;
    }
    ~AvData()
    {
        if (avData)
            delete[] avData;
    }

};

#endif