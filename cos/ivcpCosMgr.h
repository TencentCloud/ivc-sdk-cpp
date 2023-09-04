
#ifndef _IVCP_COS_MGR_H__
#define _IVCP_COS_MGR_H__

#include "ivcpCos.h"
#include <map>
#include "ivcpAvData.h"
#include <memory>

class IvcpCosMgr {
public:
    IvcpCosMgr() : _ConstCosOjb(std::make_shared<IvcpCos>()) {}
    ~IvcpCosMgr() { _CosObj.clear(); };

public:
    void CloseStreamToCos(int channelid);
    bool CreateStreamToCos(int channelid, std::string streamid, std::string planid);

public:
    void OnAVData(std::shared_ptr<AvData> avData);
    int OnImageData(const unsigned char* imageData, int imageDataLen, std::string& url);
    int OnVideoSliceData(const unsigned char* videoSliceData, int videoSliceDataLen, std::string& url);

private:
    std::map<int, std::shared_ptr<IvcpCos>> _CosObj;
    std::shared_ptr<IvcpCos>_ConstCosOjb;
    IvcpCosStatus _CosStatus;
    pthread_rwlock_t _Lock;
};
#endif