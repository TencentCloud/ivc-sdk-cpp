#ifndef _IVCP_IVCPP2P_H__
#define _IVCP_IVCPP2P_H__

#include <string>
#include <mutex>
#include "ivcpMsgStruct.h"
#include <memory>
#include <thread>
#include "ivcpCacheQueue.h"
#include "ivcpAvData.h"
#include "Network.h"
#include "flv-muxer.h"

class IvcpP2P
{
public:
    IvcpP2P(std::shared_ptr<network::UdpSocket> sock, std::string playerID, std::string playerIP, int playerPort);
    ~IvcpP2P();

public:
    //从mgr过来的数据
    void OnAVData(std::shared_ptr<AvData> avData);
public:
    //打洞
    int DoConnect();
    //发送数据
    int SendData(const char* data, int len);
private:
    //处理缓冲数据线程
    void  DealAvData();
private:
    CacheQueue<std::shared_ptr<AvData>> m_avData;
    std::shared_ptr<std::thread> m_thread;
    bool m_exited;
    std::string playerID;
    std::string playerIP;
    int playerPort;
    std::shared_ptr<network::UdpSocket> socket;

    flv_muxer_t* m_flvMuxer;
};

#endif
