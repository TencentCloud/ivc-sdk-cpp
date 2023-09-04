#include "ivcpCosMgr.h"
#include "ivcpMsgType.h"

void IvcpCosMgr::OnAVData(std::shared_ptr<AvData> avData)
{
    if (_CosStatus.GetCosIOStatus() == false)
    {
        if (_CosStatus.ReInitCosIO() == false) return;
    }

    pthread_rwlock_rdlock(&_Lock);
    auto iter = _CosObj.find(avData->channelID);
    if (iter == _CosObj.end())
    {
        pthread_rwlock_unlock(&_Lock);
        return;
    }

    iter->second->OnAVData(avData);
    pthread_rwlock_unlock(&_Lock);
}

int IvcpCosMgr::OnImageData(const unsigned char* imageData, int imageDataLen, std::string& url)
{
    if (_CosStatus.GetCosIOStatus() == false)
    {
        if (_CosStatus.ReInitCosIO() == false) return CosClientError;
    }

    return  _ConstCosOjb->UploadImage(imageData, imageDataLen, url);
}

int IvcpCosMgr::OnVideoSliceData(const unsigned char* videoSliceData, int videoSliceDataLen, std::string& url)
{
    if (_CosStatus.GetCosIOStatus() == false)
    {
        if (_CosStatus.ReInitCosIO() == false) return CosClientError;
    }
    return _ConstCosOjb->UploadVideoSlice(videoSliceData, videoSliceDataLen, url);
}

void IvcpCosMgr::CloseStreamToCos(int channelID)
{
    pthread_rwlock_wrlock(&_Lock);
    auto iter = _CosObj.find(channelID);
    if (iter == _CosObj.end())
    {
        pthread_rwlock_unlock(&_Lock);
        return;
    }
    _CosObj.erase(iter);
    pthread_rwlock_unlock(&_Lock);
}

bool IvcpCosMgr::CreateStreamToCos(int channelid, std::string streamid, std::string planid)
{
    if (_CosStatus.GetCosIOStatus() == false)
    {
        if (_CosStatus.ReInitCosIO() == false) return false;
    }
    {
        pthread_rwlock_rdlock(&_Lock);
        auto iter = _CosObj.find(channelid);
        if (iter != _CosObj.end())
        {
            pthread_rwlock_unlock(&_Lock);
            return false;
        }
        pthread_rwlock_unlock(&_Lock);
    }


    auto cosObj = std::make_shared<IvcpCos>(streamid, planid);

    int ret = cosObj->TouchBucket();

    if (ret == Success)
    {
        pthread_rwlock_wrlock(&_Lock);
        _CosObj.insert(std::make_pair(channelid, cosObj));
        pthread_rwlock_unlock(&_Lock);
        return true;
    }
    else
    {
        return false;
    }
}