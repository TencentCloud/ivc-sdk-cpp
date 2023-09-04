
#ifndef _IVCP_RTMP_MGR_H__
#define _IVCP_RTMP_MGR_H__

#include "ivcpRtmp.h"
#include <map>
#include "ivcpAvData.h"
#include <memory>

class IvcpRtmpMgr {
public:
    IvcpRtmpMgr() {};
    ~IvcpRtmpMgr() { _RtmpObj.clear(); };

public:
    void OnAVData(std::shared_ptr<AvData> avData);

    void CloseRtmp(int channelID);
    bool CreateRtmp(int channelID, std::string url);
private:
    std::map<int, std::shared_ptr<IvcpRtmpStream>> _RtmpObj;

    pthread_rwlock_t _Lock;
};
#endif