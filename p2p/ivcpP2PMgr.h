
#ifndef _IVCP_P2P_MGR_H__
#define _IVCP_P2P_MGR_H__

#include "ivcpP2P.h"
#include <map>
#include "ivcpAvData.h"
#include <memory>
#include "Discovery.h"

class IvcpP2PMgr {
public:
    IvcpP2PMgr()
    {
        _NatInfo.NatType = NAT_Unknown;
        _NatInfo.NatIp = "";
        _NatInfo.NatPort = -1;

        {
            NatName[NAT_Open] = "NAT_Open";
            NatName[NAT_FullCone] = "NAT_FullCone";
            NatName[NAT_IpRestrictedCone] = "NAT_IpRestrictedCone";
            NatName[NAT_PortRestrictedCone] = "NAT_PortRestrictedCone";
            NatName[NAT_Symmetric] = "NAT_Symmetric";
            NatName[NAT_Blockled] = "NAT_Blockled";
            NatName[NAT_Unknown] = "NAT_Unknown";
        }

        _Socket = std::make_shared<network::UdpSocket>();
        Disc = std::make_shared<stun::Discovery>(_Socket, "stun.ivc.myqcloud.com", 11996);
    };
    ~IvcpP2PMgr() { _P2PObj.clear(); }


public:
    void OnAVData(std::shared_ptr<AvData> avData);

    void CloseP2P(int channelID, std::string playerID);
    int CreateP2P(int channelID, std::string playerID, std::string playerIP, int playerPort);

public:
    void DiscoverNatInfo();
    void SetNatInfo(NatInfo info);;
    NatInfo GetNatInfo();

private:
    std::map<int, std::map<std::string, std::shared_ptr<IvcpP2P>>> _P2PObj;
    NatInfo _NatInfo;
    std::shared_ptr<stun::Discovery>Disc;
    std::map<int, std::string> NatName; // NatType <->  NatName
    std::shared_ptr<network::UdpSocket> _Socket;
    pthread_rwlock_t _Lock;
};
#endif