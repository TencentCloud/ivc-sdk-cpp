#include "ivcpP2P.h"
#include "Buffer.h"
#include "ivcpData.h"
#include <vector>
#include "ivcpMsgType.h"
#include "flv-writer.h"

struct my_flv_writer_t
{
    FILE* fp;
    flv_writer_onwrite write;
    void* param;
};

//flv 是 flv_muxer_t 的param(即：my_flv_writer_t)
//my_flv_writer_t的param 是this指针
static int flv_onmuxer(void* flv, int type, const void* data, size_t bytes, uint32_t timestamp)
{
    return flv_writer_input(flv, type, data, bytes, timestamp);
}

static int my_file_write(void* param, const struct flv_vec_t* vec, int n)
{
    IvcpP2P* obj = static_cast<IvcpP2P*>(param);

    for (int i = 0; i < n; i++)
    {
        /*printf("write %d bytes:", vec[i].len);
          char* ptr = (char*)vec[i].ptr;
          for (int j = 0; j < vec[i].len; j++)
          {
              printf("%x", ptr[j]);
          }
          printf("\n");*/

        obj->SendData((const char*)vec[i].ptr, vec[i].len);
    }
    return 0;
}

void* my_flv_write_create(void* param)
{
    struct my_flv_writer_t* flv;

    flv = (my_flv_writer_t*)flv_writer_create2(1, 1, my_file_write, param);
    if (!flv)
    {
        return NULL;
    }

    return flv;
}

IvcpP2P::IvcpP2P(std::shared_ptr<network::UdpSocket> sock, std::string playerID, std::string playerIP, int playerPort) :m_avData(100), m_exited(false)
{
    this->playerID = playerID;
    this->playerIP = playerIP;
    this->playerPort = playerPort;
    this->socket = sock;

    m_thread = std::make_shared<std::thread>(&IvcpP2P::DealAvData, this);

    void* w = my_flv_write_create(this);
    m_flvMuxer = flv_muxer_create(flv_onmuxer, w);
}

IvcpP2P::~IvcpP2P()
{
    m_exited = true;
    m_avData.cexit();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    if (m_thread->joinable())
    {
        m_thread->join();
    }
}

void IvcpP2P::OnAVData(std::shared_ptr<AvData> avData)
{
    m_avData.add(avData);
}

void Stringsplit(const std::string& str, const std::string& splits, std::vector<std::string>& res)
{
    if (str == "")		return;
    //在字符串末尾也加入分隔符，方便截取最后一段
    std::string strs = str + splits;
    size_t pos = strs.find(splits);
    int step = splits.size();

    // 若找不到内容则字符串搜索函数返回 npos
    while (pos != strs.npos)
    {
        std::string temp = strs.substr(0, pos);
        res.push_back(temp);
        //去掉已分割的字符串,在剩下的字符串中进行分割
        strs = strs.substr(pos + step, strs.size());
        pos = strs.find(splits);
    }
}


int IvcpP2P::DoConnect()
{
    //格式：1111:ivcpp2p:devicesn:request:playerid
    std::string reqMsg = "1111:ivcpp2p:" + IvcpData::GetInstance().GetDeviceSn() + ":request:" + playerID;

    //格式：1001:ivcpp2p:devicesn:response:playerid
    std::string rspMsg = "";

    int i = 0;
    do
    {
        socket->writeTo(reqMsg.c_str(), reqMsg.length(), playerIP, playerPort);

        unsigned char buf[1024];
        ssize_t len = socket->read(buf, 1024, 1000);
        if (len > 0)
        {
            rspMsg = std::string((char*)buf);
            std::vector<std::string> vecRspMsg;
            vecRspMsg.clear();

            Stringsplit(rspMsg, ":", vecRspMsg);
            if (vecRspMsg.size() == 4)
            {
                if (vecRspMsg[0] == "ivcpp2p" && vecRspMsg[1] == "devicesn" && vecRspMsg[2] == "response")
                {
                    if (vecRspMsg[3] == playerID)
                    {
                        return 0;
                    }
                }
            }
        }
        i++;
    } while (i < 3);

    return -1;
}

int IvcpP2P::SendData(const char* data, int len)
{
    socket->writeTo(data, len, playerIP, playerPort);
}

void IvcpP2P::DealAvData()
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
            flv_muxer_avc(m_flvMuxer, obj->avData, obj->avDataLen, obj->avPts, obj->avDts);
        }
        break;
        case H265:
        {
            flv_muxer_hevc(m_flvMuxer, obj->avData, obj->avDataLen, obj->avPts, obj->avDts);
        }
        break;
        case AAC:
        {
            flv_muxer_aac(m_flvMuxer, obj->avData, obj->avDataLen, obj->avPts, obj->avDts);
        }
        break;
        case G711A:
        {
            flv_muxer_g711a(m_flvMuxer, obj->avData, obj->avDataLen, obj->avPts, obj->avDts);
        }
        break;
        case G711U:
        {
            flv_muxer_g711u(m_flvMuxer, obj->avData, obj->avDataLen, obj->avPts, obj->avDts);
        }
        break;
        default:
            break;
        }
    }
}
