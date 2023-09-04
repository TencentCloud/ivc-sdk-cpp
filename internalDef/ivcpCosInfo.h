
#ifndef _IVCP_COSSECINFO_H__
#define _IVCP_COSSECINFO_H__


#include <string>

struct CosSecInfo
{
    std::string SecretID = "";
    std::string SecretKey = "";
    std::string SessionToken = "";
    std::string Region = "";
    std::string BucketID = "";
    long long ExpireTime = 0;
};
#endif