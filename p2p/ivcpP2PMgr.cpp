#include "ivcpP2PMgr.h"
#include <glog/logging.h>
#include "ivcpMsgType.h"

void IvcpP2PMgr::OnAVData(std::shared_ptr<AvData> avData)
{
    pthread_rwlock_rdlock(&_Lock);

    auto iter = _P2PObj.find(avData->channelID);
    if (iter == _P2PObj.end())
    {
        pthread_rwlock_unlock(&_Lock);
        return;
    }
    for (auto& p2p : iter->second)
    {
        p2p.second->OnAVData(avData);
    }
    pthread_rwlock_unlock(&_Lock);
}

void IvcpP2PMgr::CloseP2P(int channelID, std::string playerID)
{
    pthread_rwlock_wrlock(&_Lock);

    auto iter = _P2PObj.find(channelID);
    if (iter == _P2PObj.end())
    {
        pthread_rwlock_unlock(&_Lock);
        return;
    }

    auto obj = iter->second.find(playerID);
    if (obj == iter->second.end())
    {
        pthread_rwlock_unlock(&_Lock);
        return;
    }
    else
    {
        iter->second.erase(obj);
    }

    if (iter->second.empty())
    {
        _P2PObj.erase(iter);
    }
    pthread_rwlock_unlock(&_Lock);
}

int IvcpP2PMgr::CreateP2P(int channelID, std::string playerID, std::string playerIP, int playerPort)
{
    //通道可能有一对多的情况
    {
        pthread_rwlock_rdlock(&_Lock);
        auto iter = _P2PObj.find(channelID);
        if (iter != _P2PObj.end())
        {
            auto obj = iter->second.find(playerID);
            //找到通道和对应的playerid
            if (obj != iter->second.end())
            {
                LOG(ERROR) << "playerID:" << playerID << " already exist";
                pthread_rwlock_unlock(&_Lock);
                return P2PSessiocExist;
            }
            else
            {
                //没有找到playerid
                auto p2p = std::make_shared<IvcpP2P>(_Socket, playerID, playerIP, playerPort);

                auto ret = p2p->DoConnect();
                if (ret != Success)
                {
                    LOG(ERROR) << "playerID:" << playerID << " connect fail";
                    pthread_rwlock_unlock(&_Lock);
                    return ret;
                }
                iter->second[playerID] = p2p;
                pthread_rwlock_unlock(&_Lock);
                return Success;
            }
        }

        pthread_rwlock_unlock(&_Lock);
    }


    auto p2p = std::make_shared<IvcpP2P>(_Socket, playerID, playerIP, playerPort);
    auto ret = p2p->DoConnect();
    if (ret != Success)
    {
        LOG(ERROR) << "playerID:" << playerID << " connect fail";

        return ret;
    }

    {
        pthread_rwlock_wrlock(&_Lock);
        std::map<std::string, std::shared_ptr<IvcpP2P> > se;
        se.insert(std::make_pair(playerID, p2p));

        _P2PObj.insert(std::make_pair(channelID, se));
        pthread_rwlock_unlock(&_Lock);
    }

    return Success;
}

NatInfo IvcpP2PMgr::GetNatInfo()
{
    return _NatInfo;
}

void IvcpP2PMgr::SetNatInfo(NatInfo info)
{
    _NatInfo.NatIp = info.NatIp;
    _NatInfo.NatPort = info.NatPort;
    _NatInfo.NatType = info.NatType;
    _NatInfo.NatName = NatName[_NatInfo.NatType];

    LOG(INFO) << "NatType:" << NatName[_NatInfo.NatType] << ", NatIp:" << _NatInfo.NatIp << ", NatPort:" << _NatInfo.NatPort;
}

void IvcpP2PMgr::DiscoverNatInfo()
{
    std::thread discoverThread([this]() {
        Disc->discover(this);
        });

    if (discoverThread.joinable())
    {
        discoverThread.join();
    }
}

