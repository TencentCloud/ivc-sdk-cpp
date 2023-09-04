#include "ivcpRtmpMgr.h"

void IvcpRtmpMgr::OnAVData(std::shared_ptr<AvData> avData)
{
    pthread_rwlock_rdlock(&_Lock);

    auto iter = _RtmpObj.find(avData->channelID);
    if (iter == _RtmpObj.end())
    {
        pthread_rwlock_unlock(&_Lock);
        return;
    }

    iter->second->OnAVData(avData);

    pthread_rwlock_unlock(&_Lock);
}

void IvcpRtmpMgr::CloseRtmp(int channelID)
{
    pthread_rwlock_wrlock(&_Lock);

    auto iter = _RtmpObj.find(channelID);
    if (iter == _RtmpObj.end())
    {
        pthread_rwlock_unlock(&_Lock);
        return;
    }
    _RtmpObj.erase(iter);

    pthread_rwlock_unlock(&_Lock);
}

bool IvcpRtmpMgr::CreateRtmp(int channelID, std::string url)
{
    {
        pthread_rwlock_rdlock(&_Lock);

        auto iter = _RtmpObj.find(channelID);
        if (iter != _RtmpObj.end())
        {
            pthread_rwlock_unlock(&_Lock);
            return false;
        }
        pthread_rwlock_unlock(&_Lock);
    }

    auto rtmpObj = std::make_shared<IvcpRtmpStream>();
    bool ret = rtmpObj->Connect(url.c_str());
    if (false == ret)
    {
        return false;
    }

    {
        pthread_rwlock_wrlock(&_Lock);

        _RtmpObj.insert(std::make_pair(channelID, rtmpObj));

        pthread_rwlock_unlock(&_Lock);
    }
}