#include "ivcpRtmp.h"
#include "ivcpSpsDecode.h"
#include <cstring>
#include "ivcpData.h"
#include "ivcpMsgType.h"
#include "ot.h"
namespace ivcpRtmp
{
    enum
    {
        FLV_CODECID_H264 = 7,
        FLV_CODECID_H265 = 12,
    };
    enum
    {
        FLV_CODECID_G711A = 7,
        FLV_CODECID_G711U = 8,
        FLV_CODECID_AAC = 10,
    };

    int InitSockets()
    {
#ifdef WIN32
        WORD version;
        WSADATA wsaData;
        version = MAKEWORD(1, 1);
        return (WSAStartup(version, &wsaData) == 0);
#else
        return true;
#endif
    }

    inline void CleanupSockets()
    {
#ifdef WIN32
        WSACleanup();
#endif
    }

    char* put_byte(char* output, uint8_t nVal)
    {
        output[0] = nVal;
        return output + 1;
    }
    char* put_be16(char* output, uint16_t nVal)
    {
        output[1] = nVal & 0xff;
        output[0] = nVal >> 8;
        return output + 2;
    }
    char* put_be24(char* output, uint32_t nVal)
    {
        output[2] = nVal & 0xff;
        output[1] = nVal >> 8;
        output[0] = nVal >> 16;
        return output + 3;
    }
    char* put_be32(char* output, uint32_t nVal)
    {
        output[3] = nVal & 0xff;
        output[2] = nVal >> 8;
        output[1] = nVal >> 16;
        output[0] = nVal >> 24;
        return output + 4;
    }
    char* put_be64(char* output, uint64_t nVal)
    {
        output = put_be32(output, nVal >> 32);
        output = put_be32(output, nVal);
        return output;
    }
    char* put_amf_string(char* c, const char* str)
    {
        uint16_t len = strlen(str);
        c = put_be16(c, len);
        memcpy(c, str, len);
        return c + len;
    }
    char* put_amf_double(char* c, double d)
    {
        *c++ = AMF_NUMBER; /* type: Number */
        {
            unsigned char* ci, * co;
            ci = (unsigned char*)&d;
            co = (unsigned char*)c;
            co[0] = ci[7];
            co[1] = ci[6];
            co[2] = ci[5];
            co[3] = ci[4];
            co[4] = ci[3];
            co[5] = ci[2];
            co[6] = ci[1];
            co[7] = ci[0];
        }
        return c + 8;
    }
};

IvcpRtmpStream::IvcpRtmpStream(void) : m_nFileBufSize(0),
m_nCurPos(0), m_avData(100), m_exited(false)
{
    ivcpRtmp::InitSockets();
    m_pRtmp = RTMP_Alloc();
    RTMP_Init(m_pRtmp);

    m_thread = std::make_shared<std::thread>(&IvcpRtmpStream::DealAvData, this);
}

IvcpRtmpStream::~IvcpRtmpStream(void)
{
    m_exited = true;
    m_avData.cexit();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    if (m_thread->joinable())
    {
        m_thread->join();
    }

    RTMP_Free(m_pRtmp);
    Close();
    ivcpRtmp::CleanupSockets();
}

bool IvcpRtmpStream::Connect(const char* url)
{
    if (RTMP_SetupURL(m_pRtmp, (char*)url) < 0)
    {
        return false;
    }
    RTMP_EnableWrite(m_pRtmp);
    if (RTMP_Connect(m_pRtmp, NULL) < 0)
    {
        return false;
    }
    if (RTMP_ConnectStream(m_pRtmp, 2) < 0)
    {
        return false;
    }
    return true;
}

void IvcpRtmpStream::Close()
{
    if (m_pRtmp)
    {
        RTMP_Close(m_pRtmp);
        RTMP_Free(m_pRtmp);
        m_pRtmp = NULL;
    }
}

int IvcpRtmpStream::SendPacket(unsigned int nPacketType, unsigned char* data, unsigned int size, unsigned int nTimestamp)
{
    if (m_pRtmp == NULL)
    {
        return false;
    }

    RTMPPacket packet;
    RTMPPacket_Reset(&packet);
    RTMPPacket_Alloc(&packet, size);

    packet.m_packetType = nPacketType;
    packet.m_nChannel = 0x04;
    packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet.m_nTimeStamp = nTimestamp;
    packet.m_nInfoField2 = m_pRtmp->m_stream_id;
    packet.m_nBodySize = size;
    memcpy(packet.m_body, data, size);

    int nRet = RTMP_SendPacket(m_pRtmp, &packet, 0);

    RTMPPacket_Free(&packet);

    return nRet;
}

bool IvcpRtmpStream::SendH264Metadata(LPVideoMetadata lpMetaData)
{
    if (lpMetaData == NULL)
    {
        return false;
    }
    char* body = new char[lpMetaData->nSpsLen + lpMetaData->nPpsLen + 1024];

    char* p = (char*)body;
    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "@setDataFrame");

    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "onMetaData");

    p = ivcpRtmp::put_byte(p, AMF_OBJECT);
    p = ivcpRtmp::put_amf_string(p, "copyright");
    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "firehood");

    p = ivcpRtmp::put_amf_string(p, "width");
    p = ivcpRtmp::put_amf_double(p, lpMetaData->nWidth);

    p = ivcpRtmp::put_amf_string(p, "height");
    p = ivcpRtmp::put_amf_double(p, lpMetaData->nHeight);

    p = ivcpRtmp::put_amf_string(p, "framerate");
    p = ivcpRtmp::put_amf_double(p, lpMetaData->nFrameRate);

    p = ivcpRtmp::put_amf_string(p, "videocodecid");
    p = ivcpRtmp::put_amf_double(p, ivcpRtmp::FLV_CODECID_H264);

    p = ivcpRtmp::put_amf_string(p, "");
    p = ivcpRtmp::put_byte(p, AMF_OBJECT_END);

    int index = p - body;

    SendPacket(RTMP_PACKET_TYPE_INFO, (unsigned char*)body, p - body, 0);

    int i = 0;
    body[i++] = 0x17; // 1:keyframe  7:AVC
    body[i++] = 0x00; // AVC sequence header

    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00; // fill in 0;

    // AVCDecoderConfigurationRecord.
    body[i++] = 0x01;               // configurationVersion
    body[i++] = lpMetaData->Sps[1]; // AVCProfileIndication
    body[i++] = lpMetaData->Sps[2]; // profile_compatibility
    body[i++] = lpMetaData->Sps[3]; // AVCLevelIndication
    body[i++] = 0xff;               // lengthSizeMinusOne

    // sps nums
    body[i++] = 0xE1; //&0x1f
    // sps data length
    body[i++] = lpMetaData->nSpsLen >> 8;
    body[i++] = lpMetaData->nSpsLen & 0xff;
    // sps data
    memcpy(&body[i], lpMetaData->Sps, lpMetaData->nSpsLen);
    i = i + lpMetaData->nSpsLen;

    // pps nums
    body[i++] = 0x01; //&0x1f
    // pps data length
    body[i++] = lpMetaData->nPpsLen >> 8;
    body[i++] = lpMetaData->nPpsLen & 0xff;
    // sps data
    memcpy(&body[i], lpMetaData->Pps, lpMetaData->nPpsLen);
    i = i + lpMetaData->nPpsLen;

    int ret = SendPacket(RTMP_PACKET_TYPE_VIDEO, (unsigned char*)body, i, 0);

    delete[] body;
    return ret;
}

bool IvcpRtmpStream::SendH265Metadata(LPVideoMetadata lpMetaData)
{
    if (lpMetaData == NULL)
    {
        return false;
    }
    char* body = new char[lpMetaData->nSpsLen + lpMetaData->nPpsLen + lpMetaData->nVpsLen + 1024];

    char* p = (char*)body;
    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "@setDataFrame");

    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "onMetaData");

    p = ivcpRtmp::put_byte(p, AMF_OBJECT);
    p = ivcpRtmp::put_amf_string(p, "copyright");
    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "firehood");

    p = ivcpRtmp::put_amf_string(p, "width");
    p = ivcpRtmp::put_amf_double(p, lpMetaData->nWidth);

    p = ivcpRtmp::put_amf_string(p, "height");
    p = ivcpRtmp::put_amf_double(p, lpMetaData->nHeight);

    p = ivcpRtmp::put_amf_string(p, "framerate");
    p = ivcpRtmp::put_amf_double(p, lpMetaData->nFrameRate);

    p = ivcpRtmp::put_amf_string(p, "videocodecid");
    p = ivcpRtmp::put_amf_double(p, ivcpRtmp::FLV_CODECID_H265);

    p = ivcpRtmp::put_amf_string(p, "");
    p = ivcpRtmp::put_byte(p, AMF_OBJECT_END);

    int index = p - body;

    SendPacket(RTMP_PACKET_TYPE_INFO, (unsigned char*)body, p - body, 0);

    int i = 0;
    body[i++] = 0x1C; // 1:keyframe  7:AVC   C:HEVC
    body[i++] = 0x00; // HEVC sequence header
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00; // fill in 0;

    // general_profile_idc 8bit
    body[i++] = lpMetaData->Sps[1];
    // general_profile_compatibility_flags 32 bit
    body[i++] = lpMetaData->Sps[2];
    body[i++] = lpMetaData->Sps[3];
    body[i++] = lpMetaData->Sps[4];
    body[i++] = lpMetaData->Sps[5];

    // 48 bit NUll nothing deal in rtmp
    body[i++] = lpMetaData->Sps[6];
    body[i++] = lpMetaData->Sps[7];
    body[i++] = lpMetaData->Sps[8];
    body[i++] = lpMetaData->Sps[9];
    body[i++] = lpMetaData->Sps[10];
    body[i++] = lpMetaData->Sps[11];

    // general_level_idc
    body[i++] = lpMetaData->Sps[12];

    // 48 bit NUll nothing deal in rtmp
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;

    // bit(16) avgFrameRate;
    body[i++] = 0x00;
    body[i++] = 0x00;

    /* bit(2) constantFrameRate; */
    /* bit(3) numTemporalLayers; */
    /* bit(1) temporalIdNested; */
    body[i++] = 0x00;

    /* unsigned int(8) numOfArrays; 03 */
    body[i++] = 0x03;

    // vps
    body[i++] = 0x20; // vps 32
    body[i++] = (1 >> 8) & 0xff;
    body[i++] = 1 & 0xff;
    body[i++] = (lpMetaData->nVpsLen >> 8) & 0xff;
    body[i++] = (lpMetaData->nVpsLen) & 0xff;
    memcpy(&body[i], lpMetaData->Vps, lpMetaData->nVpsLen);
    i += lpMetaData->nVpsLen;

    // sps
    body[i++] = 0x21; // sps 33
    body[i++] = (1 >> 8) & 0xff;
    body[i++] = 1 & 0xff;
    body[i++] = (lpMetaData->nSpsLen >> 8) & 0xff;
    body[i++] = lpMetaData->nSpsLen & 0xff;
    memcpy(&body[i], lpMetaData->Sps, lpMetaData->nSpsLen);
    i += lpMetaData->nSpsLen;

    // pps
    body[i++] = 0x22; // pps 34
    body[i++] = (1 >> 8) & 0xff;
    body[i++] = 1 & 0xff;
    body[i++] = (lpMetaData->nPpsLen >> 8) & 0xff;
    body[i++] = (lpMetaData->nPpsLen) & 0xff;
    memcpy(&body[i], lpMetaData->Pps, lpMetaData->nPpsLen);
    i += lpMetaData->nPpsLen;

    int ret = SendPacket(RTMP_PACKET_TYPE_VIDEO, (unsigned char*)body, i, 0);

    delete[] body;
    return ret;
}

bool IvcpRtmpStream::SendAACMetaData(LPAudioMetadata lpMetaData)
{
    if (lpMetaData == NULL)
    {
        return false;
    }
    char* body = new char[lpMetaData->nAudioSpecCfgLen + 1024];

    char* p = (char*)body;
    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "@setDataFrame");

    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "onMetaData");

    p = ivcpRtmp::put_byte(p, AMF_OBJECT);
    p = ivcpRtmp::put_amf_string(p, "copyright");
    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "firehood");

    p = ivcpRtmp::put_amf_string(p, "audiosamplerate");
    p = ivcpRtmp::put_amf_double(p, lpMetaData->nSamplerate);

    p = ivcpRtmp::put_amf_string(p, "audiocodecid");
    p = ivcpRtmp::put_amf_double(p, ivcpRtmp::FLV_CODECID_AAC);

    p = ivcpRtmp::put_amf_string(p, "");
    p = ivcpRtmp::put_byte(p, AMF_OBJECT_END);

    int index = p - body;

    SendPacket(RTMP_PACKET_TYPE_INFO, (unsigned char*)body, p - body, 0);

    body[0] = int(lpMetaData->nAudioFormat) << 4 | 0x0f;
    body[1] = 0x00;
    memcpy(&body[2], lpMetaData->pAudioSpecCfg, lpMetaData->nAudioSpecCfgLen);

    int ret = SendPacket(RTMP_PACKET_TYPE_AUDIO, (unsigned char*)body, lpMetaData->nAudioSpecCfgLen + 2, 0);

    delete[] body;
    return ret;
}

bool IvcpRtmpStream::Send711AMetaData(LPAudioMetadata lpMetaData)
{
    if (lpMetaData == NULL)
    {
        return false;
    }
    char* body = new char[lpMetaData->nAudioSpecCfgLen + 1024];

    char* p = (char*)body;
    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "@setDataFrame");

    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "onMetaData");

    p = ivcpRtmp::put_byte(p, AMF_OBJECT);
    p = ivcpRtmp::put_amf_string(p, "copyright");
    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "firehood");

    p = ivcpRtmp::put_amf_string(p, "audiosamplerate");
    p = ivcpRtmp::put_amf_double(p, lpMetaData->nSamplerate);

    p = ivcpRtmp::put_amf_string(p, "audiocodecid");
    p = ivcpRtmp::put_amf_double(p, ivcpRtmp::FLV_CODECID_G711A);

    p = ivcpRtmp::put_amf_string(p, "");
    p = ivcpRtmp::put_byte(p, AMF_OBJECT_END);

    int index = p - body;

    SendPacket(RTMP_PACKET_TYPE_INFO, (unsigned char*)body, p - body, 0);

    body[0] = int(lpMetaData->nAudioFormat) << 4 | 0x0f;
    body[1] = 0x00;
    memcpy(&body[2], lpMetaData->pAudioSpecCfg, lpMetaData->nAudioSpecCfgLen);

    int ret = SendPacket(RTMP_PACKET_TYPE_AUDIO, (unsigned char*)body, lpMetaData->nAudioSpecCfgLen + 2, 0);

    delete[] body;
    return ret;
}
bool IvcpRtmpStream::Send711UMetaData(LPAudioMetadata lpMetaData)
{
    if (lpMetaData == NULL)
    {
        return false;
    }
    char* body = new char[lpMetaData->nAudioSpecCfgLen + 1024];

    char* p = (char*)body;
    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "@setDataFrame");

    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "onMetaData");

    p = ivcpRtmp::put_byte(p, AMF_OBJECT);
    p = ivcpRtmp::put_amf_string(p, "copyright");
    p = ivcpRtmp::put_byte(p, AMF_STRING);
    p = ivcpRtmp::put_amf_string(p, "firehood");

    p = ivcpRtmp::put_amf_string(p, "audiosamplerate");
    p = ivcpRtmp::put_amf_double(p, lpMetaData->nSamplerate);

    p = ivcpRtmp::put_amf_string(p, "audiocodecid");
    p = ivcpRtmp::put_amf_double(p, ivcpRtmp::FLV_CODECID_G711U);

    p = ivcpRtmp::put_amf_string(p, "");
    p = ivcpRtmp::put_byte(p, AMF_OBJECT_END);

    int index = p - body;

    SendPacket(RTMP_PACKET_TYPE_INFO, (unsigned char*)body, p - body, 0);

    body[0] = int(lpMetaData->nAudioFormat) << 4 | 0x0f;
    body[1] = 0x00;
    memcpy(&body[2], lpMetaData->pAudioSpecCfg, lpMetaData->nAudioSpecCfgLen);

    int ret = SendPacket(RTMP_PACKET_TYPE_AUDIO, (unsigned char*)body, lpMetaData->nAudioSpecCfgLen + 2, 0);

    delete[] body;
    return ret;
}

bool IvcpRtmpStream::SendH264Packet(int channelid, unsigned char* data, unsigned int size, bool bIsKeyFrame, unsigned int nTimeStamp)
{
    if (data == NULL && size < 11)
    {
        return false;
    }

    unsigned char* body = new unsigned char[size + 9];

    int i = 0;
    if (bIsKeyFrame)
    {
        auto MetaData = IvcpData::GetInstance().GetVideoMetaData(channelid);
        if (MetaData)
        {
            int ret = SendH264Metadata(MetaData);
            FreeVideoMeta(MetaData);
            body[i++] = 0x17; // 1:Iframe  7:AVC
        }
        else
        {
            return false;
        }
    }
    else
    {
        body[i++] = 0x27; // 2:Pframe  7:AVC
    }
    body[i++] = 0x01; // AVC NALU
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;

    // NALU size
    body[i++] = size >> 24;
    body[i++] = size >> 16;
    body[i++] = size >> 8;
    body[i++] = size & 0xff;

    // NALU data
    memcpy(&body[i], data, size);

    bool bRet = SendPacket(RTMP_PACKET_TYPE_VIDEO, body, i + size, nTimeStamp);

    delete[] body;

    return bRet;
}

bool IvcpRtmpStream::SendH265Packet(int channelid, unsigned char* data, unsigned int size, bool bIsKeyFrame, unsigned int nTimeStamp)
{
    if (data == NULL && size < 11)
    {
        return false;
    }

    unsigned char* body = new unsigned char[size + 9];

    int i = 0;
    if (bIsKeyFrame)
    {
        auto MetaData = IvcpData::GetInstance().GetVideoMetaData(channelid);
        if (MetaData)
        {
            int ret = SendH265Metadata(MetaData);
            FreeVideoMeta(MetaData);
            body[i++] = 0x1C;
        }
        else
        {
            return false;
        }
    }
    else
    {
        body[i++] = 0x2C;
    }
    body[i++] = 0x01; // AVC NALU
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;

    // NALU size
    body[i++] = size >> 24 & 0xff;
    body[i++] = size >> 16 & 0xff;
    body[i++] = size >> 8 & 0xff;
    body[i++] = size & 0xff;

    // NALU data
    memcpy(&body[i], data, size);

    bool bRet = SendPacket(RTMP_PACKET_TYPE_VIDEO, body, i + size, nTimeStamp);

    delete[] body;

    return bRet;
}

bool IvcpRtmpStream::SendAACPacket(int channelid, const unsigned char* data, unsigned int size, unsigned int nTimeStamp)
{
    if (data == NULL && size < 7)
    {
        return false;
    }

    auto aacMetaData = IvcpData::GetInstance().GetAudioMetaData(channelid);
    if (aacMetaData)
    {
        int ret = SendAACMetaData(aacMetaData);
        FreeAudioMeta(aacMetaData);
    }
    else
    {
        return false;
    }

    unsigned char* body = new unsigned char[size + 10];
    memset(body, 0, size + 10);

    body[0] = 0xAF;
    body[1] = 0x01;
    memcpy(&body[2], data, size);

    int ret = SendPacket(RTMP_PACKET_TYPE_AUDIO, (unsigned char*)body, size + 2, nTimeStamp);

    delete[] body;
    return ret;
}

bool IvcpRtmpStream::SendG711UPacket(int channelid, const unsigned char* data, unsigned int size, unsigned int nTimeStamp)
{
    if (data == NULL && size < 7)
    {
        return false;
    }

    auto auMetaData = IvcpData::GetInstance().GetAudioMetaData(channelid);
    if (auMetaData)
    {
        int ret = Send711UMetaData(auMetaData);
        FreeAudioMeta(auMetaData);
    }
    else
    {
        return false;
    }

    unsigned char* body = new unsigned char[size + 10];
    memset(body, 0, size + 10);

    body[0] = 0x87;
    body[1] = 0x01;
    memcpy(&body[2], data, size);

    int ret = SendPacket(RTMP_PACKET_TYPE_AUDIO, (unsigned char*)body, size + 2, nTimeStamp);

    delete[] body;
    return ret;
}

bool IvcpRtmpStream::SendG711APacket(int channelid, const unsigned char* data, unsigned int size, unsigned int nTimeStamp)
{
    if (data == NULL && size < 7)
    {
        return false;
    }

    auto auMetaData = IvcpData::GetInstance().GetAudioMetaData(channelid);
    if (auMetaData)
    {
        int ret = Send711AMetaData(auMetaData);
        FreeAudioMeta(auMetaData);
    }
    else
    {
        return false;
    }

    unsigned char* body = new unsigned char[size + 10];
    memset(body, 0, size + 10);

    body[0] = 0x77;
    body[1] = 0x01;
    memcpy(&body[2], data, size);

    int ret = SendPacket(RTMP_PACKET_TYPE_AUDIO, (unsigned char*)body, size + 2, nTimeStamp);

    delete[] body;
    return ret;
}


void IvcpRtmpStream::OnAVData(std::shared_ptr<AvData> avData)
{
    std::cout << "  OnAVData datalen  " << avData->avDataLen << "  iskey " << avData->isKeyFrame << std::endl;
    m_avData.add(avData);
}

void  IvcpRtmpStream::DealAvData()
{
    while (!m_exited)
    {
        std::shared_ptr<AvData> obj = m_avData.pop();
        if (obj == nullptr)
        {
            continue;
        }
        switch (obj->avDataType)
        {
        case H264:
        {
            std::cout << "  datalen  " << obj->avDataLen << "  iskey " << obj->isKeyFrame << std::endl;
            if (obj->isKeyFrame) // i frame
            {
                SendH264Packet(obj->channelID, obj->avData, obj->avDataLen, true, obj->avDts);
            }
            else
            {
                SendH264Packet(obj->channelID, obj->avData, obj->avDataLen, false, obj->avDts);
            }
        }
        break;
        case H265:
        {
            if (obj->isKeyFrame) // i frame
            {
                SendH265Packet(obj->channelID, obj->avData, obj->avDataLen, true, obj->avDts);
            }
            else
            {
                SendH265Packet(obj->channelID, obj->avData, obj->avDataLen, false, obj->avDts);
            }
        }
        break;
        case AAC:
            //SendAACPacket(obj->channelID, obj->avData, obj->avDataLen, obj->avDts);
            break;
        case G711U:
            SendG711UPacket(obj->channelID, obj->avData, obj->avDataLen, obj->avDts);
            break;
        case G711A:
            SendG711APacket(obj->channelID, obj->avData, obj->avDataLen, obj->avDts);
            break;
        default:
            break;
        }
    }
}
