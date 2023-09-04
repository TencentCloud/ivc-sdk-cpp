#include "ivcpCos.h"
#include <cos_c_sdk/cos_http_io.h>
#include <cos_c_sdk/cos_api.h>
#include <cos_c_sdk/cos_log.h>
#include "ivcpData.h"
#include <glog/logging.h>
#include "ivcpMsgType.h"
#include "ot.h"

IvcpCosStatus::IvcpCosStatus()
{
    cos_log_set_level(COS_LOG_WARN);

    // set log output, default stderr
    cos_log_set_output(NULL);

    IOStatus = false;
    if (cos_http_io_initialize(NULL, 0) != COSE_OK)
    {
        IOStatus = false;
    }
    else
    {
        IOStatus = true;
    }
}
IvcpCosStatus::~IvcpCosStatus()
{
    cos_http_io_deinitialize();
}

bool IvcpCosStatus::GetCosIOStatus()
{
    return IOStatus;
}

bool  IvcpCosStatus::ReInitCosIO()
{
    if (cos_http_io_initialize(NULL, 0) != COSE_OK)
    {
        IOStatus = false;
    }
    else
    {
        IOStatus = true;
    }

    return IOStatus;
}


IvcpCos::IvcpCos(std::string streamid, std::string planid, bool needThread) :m_avData(100), m_exited(false)
{
    m_StreamId = streamid;
    m_PlanId = planid;

#ifdef UTEST
    PreEnd = "cos-internal.";
#else
    PreEnd = "cos.";
#endif
    if (needThread)
    {
        m_thread = std::make_shared<std::thread>(&IvcpCos::DealAvData, this);
    }
    else
    {
        m_thread = nullptr;
    }

    m_hls = hls_media_create(HLS_DURATION * 1000, HlsHandler, this);
}

IvcpCos::IvcpCos() :m_avData(0), m_exited(true)
{
#ifdef UTEST
    PreEnd = "cos-internal.";
#else
    PreEnd = "cos.";
#endif
}

IvcpCos::~IvcpCos()
{
    if (m_thread)
    {
        m_exited = true;
        m_avData.cexit();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (m_thread->joinable())
        {
            m_thread->join();
        }
    }
}

int IvcpCos::UploadImage(const unsigned char* imageData, int imageDataLen, std::string& url)
{
    int ret = Success;
    cos_pool_t* p = NULL;
    int is_cname = 0;
    cos_status_t* s = NULL;
    cos_request_options_t* options = NULL;
    cos_string_t bucket;
    cos_string_t object;
    cos_table_t* resp_headers;
    int traffic_limit = 0;
    cos_list_t buffer;
    cos_buf_t* content = NULL;
    cos_table_t* headers = NULL;

    cos_pool_create(&p, NULL);
    options = cos_request_options_create(p);

    options->config = cos_config_create(options->pool);
    auto secinfo = IvcpData::GetInstance().GetCosSeCInfo();

    // std::string CosEndPoint = std::string("cos.") + secinfo.Region + std::string(".tencentcos.cn");

    std::string CosEndPoint = PreEnd + secinfo.Region + std::string(".tencentcos.cn");
    std::string Appid = secinfo.BucketID.substr(secinfo.BucketID.rfind("-") + 1);

    cos_str_set(&options->config->endpoint, CosEndPoint.c_str());
    cos_str_set(&options->config->access_key_id, secinfo.SecretID.c_str());
    cos_str_set(&options->config->access_key_secret, secinfo.SecretKey.c_str());
    cos_str_set(&options->config->sts_token, secinfo.SessionToken.c_str());

    cos_str_set(&options->config->appid, Appid.c_str());
    options->config->is_cname = is_cname;
    options->ctl = cos_http_controller_create(options->pool, 0);

    cos_str_set(&bucket, secinfo.BucketID.c_str());
    cos_str_set(&object, url.c_str());

    cos_list_init(&buffer);

    content = cos_buf_pack(options->pool, imageData, imageDataLen);
    cos_list_add_tail(&content->node, &buffer);
    s = cos_put_object_from_buffer(options, &bucket, &object, &buffer, headers, &resp_headers);

    if (s->code == 200)
    {
        ret = Success;
    }
    else
    {
        LOG(INFO) << "UploadImage, code:" << s->code << "bucket" << secinfo.BucketID << "url"
            << url << "error_code" << s->error_code << "error_msg" << s->error_msg << "req_id" << s->req_id;

        ret = WriteCosFailed;
    }

    cos_pool_destroy(p);

    return ret;
}

int IvcpCos::UploadVideoSlice(const unsigned char* videoSliceData, int videoSliceDataLen, std::string& url)
{
    int ret = Success;
    cos_pool_t* p = NULL;
    int is_cname = 0;
    cos_status_t* s = NULL;
    cos_request_options_t* options = NULL;
    cos_string_t bucket;
    cos_string_t object;
    cos_table_t* resp_headers;
    int traffic_limit = 0;
    cos_list_t buffer;
    cos_buf_t* content = NULL;
    cos_table_t* headers = NULL;

    cos_pool_create(&p, NULL);
    options = cos_request_options_create(p);

    options->config = cos_config_create(options->pool);
    auto secinfo = IvcpData::GetInstance().GetCosSeCInfo();

    std::string CosEndPoint = PreEnd + secinfo.Region + std::string(".tencentcos.cn");
    std::string Appid = secinfo.BucketID.substr(secinfo.BucketID.rfind("-") + 1);

    cos_str_set(&options->config->endpoint, CosEndPoint.c_str());
    cos_str_set(&options->config->access_key_id, secinfo.SecretID.c_str());
    cos_str_set(&options->config->access_key_secret, secinfo.SecretKey.c_str());
    cos_str_set(&options->config->sts_token, secinfo.SessionToken.c_str());
    cos_str_set(&options->config->appid, Appid.c_str());
    options->config->is_cname = is_cname;
    options->ctl = cos_http_controller_create(options->pool, 0);

    cos_str_set(&bucket, secinfo.BucketID.c_str());
    cos_str_set(&object, url.c_str());

    cos_list_init(&buffer);

    content = cos_buf_pack(options->pool, videoSliceData, videoSliceDataLen);
    cos_list_add_tail(&content->node, &buffer);
    s = cos_put_object_from_buffer(options, &bucket, &object, &buffer, headers, &resp_headers);

    if (s->code == 200)
    {
        ret = Success;
    }
    else
    {
        LOG(INFO) << "UploadVideoSlice, code:" << s->code << "bucket" << secinfo.BucketID << "url"
            << url << "error_code" << s->error_code << "error_msg" << s->error_msg << "req_id" << s->req_id;

        ret = WriteCosFailed;
    }

    cos_pool_destroy(p);

    return ret;
}

int IvcpCos::UploadVideoSliceResume(const unsigned char* videoSliceData, int videoSliceDataLen, std::string& url)
{
    cos_pool_t* p = NULL;
    char* object_name = "cos_test_append_object";
    cos_string_t bucket;
    cos_string_t object;
    char* str = "test cos c sdk";
    cos_status_t* s = NULL;
    int is_cname = 0;
    int64_t position = 0;
    cos_table_t* headers = NULL;
    cos_table_t* headers1 = NULL;
    cos_table_t* resp_headers = NULL;
    cos_request_options_t* options = NULL;
    cos_list_t buffer;
    cos_buf_t* content = NULL;
    char* next_append_position = NULL;
    auto secinfo = IvcpData::GetInstance().GetCosSeCInfo();
    secinfo.BucketID = "ivc-test-1310269012-ap-shanghai-1258344699";
    secinfo.Region = "ap-shanghai";

    std::string CosEndPoint = std::string("cos.") + secinfo.Region + std::string(".myqcloud.com");
    std::string Appid = secinfo.BucketID.substr(secinfo.BucketID.rfind("-") + 1);

    cos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    options->config = cos_config_create(options->pool);
    cos_str_set(&options->config->endpoint, CosEndPoint.c_str());
    cos_str_set(&options->config->access_key_id, secinfo.SecretID.c_str());
    cos_str_set(&options->config->access_key_secret, secinfo.SecretKey.c_str());
    cos_str_set(&options->config->appid, secinfo.SessionToken.c_str());
    options->config->is_cname = is_cname;
    options->ctl = cos_http_controller_create(options->pool, 0);

    headers = cos_table_make(p, 0);
    cos_str_set(&bucket, secinfo.BucketID.c_str());

    cos_str_set(&object, url.c_str());
    s = cos_head_object(options, &bucket, &object, headers, &resp_headers);
    if (s->code == 200)
    {
        next_append_position = (char*)(apr_table_get(resp_headers,
            "x-cos-next-append-position"));
        position = atoi(next_append_position);
    }

    /* append object */
    resp_headers = NULL;
    headers1 = cos_table_make(p, 0);
    cos_list_init(&buffer);
    content = cos_buf_pack(p, str, strlen(str));
    cos_list_add_tail(&content->node, &buffer);

    s = cos_append_object_from_buffer(options, &bucket, &object,
        position, &buffer, headers1, &resp_headers);

    cos_pool_destroy(p);
}

int IvcpCos::UploadVideoStream(const unsigned char* videoSliceData, int videoSliceDataLen, std::string& url)
{
    int ret = Success;
    cos_pool_t* p = NULL;
    int is_cname = 0;
    cos_status_t* s = NULL;
    cos_request_options_t* options = NULL;
    cos_string_t bucket;
    cos_string_t object;
    cos_table_t* resp_headers;
    int traffic_limit = 0;
    cos_list_t buffer;
    cos_buf_t* content = NULL;
    cos_table_t* headers = NULL;

    cos_pool_create(&p, NULL);
    options = cos_request_options_create(p);

    options->config = cos_config_create(options->pool);
    auto secinfo = IvcpData::GetInstance().GetCosSeCInfo();

    std::string CosEndPoint = PreEnd + secinfo.Region + std::string(".tencentcos.cn");
    std::string Appid = secinfo.BucketID.substr(secinfo.BucketID.rfind("-") + 1);

    cos_str_set(&options->config->endpoint, CosEndPoint.c_str());
    cos_str_set(&options->config->access_key_id, secinfo.SecretID.c_str());
    cos_str_set(&options->config->access_key_secret, secinfo.SecretKey.c_str());
    cos_str_set(&options->config->sts_token, secinfo.SessionToken.c_str());
    cos_str_set(&options->config->appid, Appid.c_str());
    options->config->is_cname = is_cname;
    options->ctl = cos_http_controller_create(options->pool, 0);

    cos_str_set(&bucket, secinfo.BucketID.c_str());
    cos_str_set(&object, url.c_str());

    cos_list_init(&buffer);

    content = cos_buf_pack(options->pool, videoSliceData, videoSliceDataLen);
    cos_list_add_tail(&content->node, &buffer);
    s = cos_put_object_from_buffer(options, &bucket, &object, &buffer, headers, &resp_headers);

    if (s->code == 200)
    {
        ret = Success;
    }
    else
    {
        LOG(INFO) << "UploadVideoSlice, code:" << s->code << "bucket" << secinfo.BucketID << "url"
            << url << "error_code" << s->error_code << "error_msg" << s->error_msg << "req_id" << s->req_id;

        ret = WriteCosFailed;
    }

    cos_pool_destroy(p);

    return ret;
}

int IvcpCos::HlsHandler(void* param, const void* data, size_t bytes, int64_t pts, int64_t dts, int64_t duration)
{
    auto objPtr = static_cast<IvcpCos*>(param);

    time_t now = time(NULL);
    struct tm* stime = localtime(&now);

    std::string url = objPtr->GetPlanId() + "/" +
        objPtr->GetStreamId() + "/" +
        std::to_string(stime->tm_year + 1900) + "/" +
        std::to_string(stime->tm_mon + 1) + "/" +
        std::to_string(stime->tm_mday + 1900) + "/" +
        std::to_string(stime->tm_hour) + "/" +
        std::to_string(objPtr->GetStartTime()) + "-" + std::to_string(now) + ".ts";

    objPtr->ReSetStartTime(now);

    /*FILE* fp = NULL;
     static int i = 0;
     i++;
     char name[128] = { 0 };
     snprintf(name, sizeof(name) - 1, "%d.ts", i++);
     fp = fopen(name, "wb");
     fwrite(data, 1, bytes, fp);
     fclose(fp);*/

    objPtr->UploadVideoStream((const unsigned char*)data, bytes, url);
}

int IvcpCos::TouchBucket()
{
    return Success;
    int ret = Success;
    cos_pool_t* p = NULL;
    int is_cname = 0;
    cos_status_t* s = NULL;
    cos_request_options_t* options = NULL;
    cos_string_t bucket;
    cos_table_t* resp_headers;
    cos_bucket_exist_status_e bucket_exist;

    cos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    auto secinfo = IvcpData::GetInstance().GetCosSeCInfo();
    {
        options->config = cos_config_create(options->pool);

        std::string CosEndPoint = PreEnd + secinfo.Region + std::string(".tencentcos.cn");
        std::string Appid = secinfo.BucketID.substr(secinfo.BucketID.rfind("-") + 1);

        cos_str_set(&options->config->endpoint, CosEndPoint.c_str());
        cos_str_set(&options->config->access_key_id, secinfo.SecretID.c_str());
        cos_str_set(&options->config->access_key_secret, secinfo.SecretKey.c_str());
        cos_str_set(&options->config->sts_token, secinfo.SessionToken.c_str());
        cos_str_set(&options->config->appid, Appid.c_str());
        options->config->is_cname = is_cname;
        options->ctl = cos_http_controller_create(options->pool, 0);
        cos_str_set(&bucket, secinfo.BucketID.c_str());
    }

    // 检查桶是否存在
    s = cos_check_bucket_exist(options, &bucket, &bucket_exist, &resp_headers);

    if (s->code == 200)
    {
        if (bucket_exist == COS_BUCKET_NON_EXIST)
        {
            LOG(INFO) << "TouchBucket, bucket not exist:"
                << "bucket" << secinfo.BucketID;

            return BucketNotExist;
        }
        else if (bucket_exist == COS_BUCKET_EXIST)
        {
            LOG(INFO) << "TouchBucket, bucket  exist:"
                << "bucket" << secinfo.BucketID;
            return Success;
        }
        else
        {
            LOG(INFO) << "TouchBucket, bucket not exist:"
                << "bucket" << secinfo.BucketID << "bucket_exist" << bucket_exist;

            return BucketNotExist;
        }
    }
    else
    {
        LOG(INFO) << "TouchBucket, code:" << s->code << "bucket" << secinfo.BucketID << "error_code" << s->error_code << "error_msg" << s->error_msg << "req_id" << s->req_id;

        ret = WriteCosFailed;
    }

    cos_pool_destroy(p);

    return ret;
}

void IvcpCos::OnAVData(std::shared_ptr<AvData> avData)
{
    m_avData.add(avData);
}

void IvcpCos::DealAvData()
{
    while (!m_exited)
    {
        static bool once = true;
        if (once)
        {
            m_startTime = time(nullptr);
            once = !once;
        }
        std::shared_ptr<AvData> obj = m_avData.pop();
        if (obj == nullptr)
        {
            continue;
        }
        switch (obj->avDataType)
        {
        case H264:
        {
            if (obj->isKeyFrame)
            {
                auto MetaData = IvcpData::GetInstance().GetVideoMetaData(obj->channelID);
                if (MetaData)
                {
                    hls_media_input(m_hls, PSI_STREAM_H264, MetaData->Sps, MetaData->nSpsLen, obj->avPts, obj->avDts, 0);
                    hls_media_input(m_hls, PSI_STREAM_H264, MetaData->Pps, MetaData->nPpsLen, obj->avPts, obj->avDts, 0);
                    hls_media_input(m_hls, PSI_STREAM_H264, obj->avData, obj->avDataLen, obj->avPts, obj->avDts, obj->isKeyFrame ? HLS_FLAGS_KEYFRAME : 0);
                    FreeVideoMeta(MetaData);
                }
            }
        }
        break;
        case H265:
            hls_media_input(m_hls, PSI_STREAM_H265, obj->avData, obj->avDataLen, obj->avPts, obj->avDts, obj->isKeyFrame ? HLS_FLAGS_KEYFRAME : 0);
            break;
        case AAC:
            hls_media_input(m_hls, PSI_STREAM_AAC, obj->avData, obj->avDataLen, obj->avPts, obj->avDts, 0);
            break;
        case G711A:
            hls_media_input(m_hls, PSI_STREAM_AUDIO_G711A, obj->avData, obj->avDataLen, obj->avPts, obj->avDts, 0);
            break;
        case G711U:
            hls_media_input(m_hls, PSI_STREAM_AUDIO_G711U, obj->avData, obj->avDataLen, obj->avPts, obj->avDts, 0);
            break;
        default:
            // nothing to do
            return;
        }
    }
}