#ifndef _IVCP_MSGSTRUCT_H__
#define _IVCP_MSGSTRUCT_H__

#ifdef __cplusplus
extern "C"
{
#endif
    /*
    ********************************************************************* 获取设备基本信息
    */
    struct ivcpGetDeviceBaseInfo
    {
        const char* deviceID;  // 设备ID
        const char* requestID; // 请求ID
    };
    struct ivcpGetDeviceBaseRspInfo
    {
        int code;                    // 错误码
        const char* msg;             // 错误描述
        const char* DeviceModel;     // 设备型号
        const char* Manufacturer;    // 设备厂家
        const char* FirmwareVersion; // 固件版本
        const char* DeviceName;      // 设备名称
        const char* ProtocolType;    // 协议类型，固定为"ivcp"
    };
    /*
     ********************************************************************* 获取设备通道信息
     */
    struct ivcpGetDeviceChannelInfo
    {
        const char* deviceID;  // 设备ID
        const char* requestID; // 请求ID
    };
    struct ivcpChannelInfo
    {
        char* channelName; // 通道名称
        int channelID;     // 通道ID,从1开始，保持唯一
        int channelType;   // 通道类型，0：枪机，1：半球机，2：球机
        int channelState;  // 通道状态，0：离线，1：在线，如果是非NVR设备，则通道状态和设备状态一般情况下是一致的
    };
    struct ivcpGetDeviceChannelRspInfo
    {
        int code;                                 // 错误码
        const char* msg;                          // 错误描述
        int channelNum;                           // 通道数量
        struct ivcpChannelInfo channelInfos[1024]; // 通道信息,最大支持1024个通道
    };
    /*
    ********************************************************************* 获取设备音视频参数信息
    */
    struct ivcpGetDeviceAudioVideoInfo
    {
        const char* deviceID;  // 设备ID
        const char* requestID; // 请求ID
    };

    struct ivcpAudioVideoInfo {
        const char* VideoCodecType; // 视频编码类型， h264， h265
        const char* AudioCodecType; // 音频编码类型， g.711， aac
        const char* StreamType;     // 码流类型，分为：first（第一（主）码流），second（第二（子）码流）
        const char* ComplexType;    // 复合类型，分为：complex（音视频都有），onlyvideo仅视频
        int CodeRate;               // 码率，单位Kbps，可选值：128kbps、256kbps、512kbps、1024kbps、2048kbps、4096kbps
        const char* CodeType;       // 码率类型，定码率（constant），变码率（variable）
        int Height;                 // 分辨率高 主码流可选2560*1440、1920*1080、1080*720 ；子码流可选720*576、640*360
        int Width;                  // 分辨率宽
        int Fps;                    // 帧率 ，-1代表全帧率，可选值： 1-25， -1
        int Quality;                // 图像质量 最好:5、较好:4、一般:3、较差:2、差：1
        int Gop;                    // I帧间隔 可选值：1-100
    };

    struct ivcpGetDeviceAudioVideoRspInfo
    {
        int code;                   // 错误码
        const char* msg;            // 错误描述
        int channelNum;                           // 通道数量
        struct ivcpAudioVideoInfo AvInfos[1024];
    };
    /*
     ********************************************************************* 获取设备存储参数信息
     */
    struct ivcpGetDeviceRecordInfo
    {
        const char* deviceID;  // 设备ID
        const char* requestID; // 请求ID
    };
    struct ivcpRecordInfo
    {
        int ImageNum;      // 触发事件时，截图个数，默认3，区间1到3
        int VideoDuration; // 触发事件时,录制视频长度，默认15s，区间15到30
        int ImageOnly;     // 触发事件时,是否同时录制视频，1 :不录像，2:录像。
    };

    struct ivcpGetDeviceRecordRspInfo
    {
        int code;          // 错误码
        const char* msg;   // 错误描述
        int channelNum;                           // 通道数量
        struct ivcpRecordInfo RecordInfos[1024];
    };

    /*
     ********************************************************************* 修改设备基本信息
     */
    struct ivcpModifyDeviceBaseInfo
    {
        const char* deviceID;   // 设备ID
        const char* requestID;  // 请求ID
        const char* DeviceName; // 设备名称
    };
    struct ivcpModifyDeviceBaseRspInfo
    {
        int code;        // 错误码
        const char* msg; // 错误描述
    };
    /*
    ********************************************************************* 修改设备音视频参数信息
    */
    struct ivcpModifyDeviceAudioVideoInfo
    {
        const char* deviceID;       // 设备ID
        const char* requestID;      // 请求ID
        const char* VideoCodecType; // 视频编码类型， h264， h265），如果值为空，表示保持原状
        const char* AudioCodecType; // 音频编码类型， g.711， aac），如果值为空，表示保持原状
        const char* StreamType;     // 码流类型，分为：first（第一（主）码流），second（第二（子）码流）），如果值为空，表示保持原状
        const char* ComplexType;    // 复合类型，分为：complex（音视频都有），onlyvideo仅视频），如果值为空，表示保持原状
        int CodeRate;               // 码率，单位Kbps，可选值：128kbps、256kbps、512kbps、1024kbps、2048kbps、4096kbps，如果值为0，表示保持原状
        const char* CodeType;       // 码率类型，定码率（constant），变码率（variable），如果值为空，表示保持原状
        int Height;                 // 分辨率高 主码流可选2560*1440、1920*1080、1080*720 ；子码流可选720*576、640*360，如果值为0，表示保持原状
        int Width;                  // 分辨率宽，如果值为0，表示保持原状
        int Fps;                    // 帧率 ，-1代表全帧率，可选值： 1-25， -1，如果值为0，表示保持原状
        int Quality;                // 图像质量 最好:5、较好:4、一般:3、较差:2、差：1，如果值为0，表示保持原状
        int Gop;                    // I帧间隔 可选值：1-100，如果值为0，表示保持原状
    };
    struct ivcpModifyDeviceAudioVideoRspInfo
    {
        int code;        // 错误码
        const char* msg; // 错误描述
    };
    /*
     ********************************************************************* 修改设备存储参数信息
     */
    struct ivcpModifyDeviceRecordInfo
    {
        const char* deviceID;  // 设备ID
        const char* requestID; // 请求ID
        int ImageNum;          // 触发事件时，截图个数，默认3，区间1到3，如果值为0，表示保持原状
        int VideoDuration;     // 触发事件时,录制视频长度，默认15s，区间15到30，如果值为0，表示保持原状
        int ImageOnly;         // 触发事件时,是否同时录制视频，1 :不录像，2:录像，如果值为0，表示保持原状
    };
    struct ivcpModifyDeviceRecordRspInfo
    {
        int code;        // 错误码
        const char* msg; // 错误描述
    };

    /*
    ********************************************************************* 获取设备固件版本情况
    */
    struct ivcpGetDeviceVersionInfo
    {
        const char* deviceID;  // 设备ID
        const char* requestID; // 请求ID
    };
    struct ivcpGetDeviceVersionRspInfo
    {
        int code;                           // 错误码
        const char* msg;                    // 错误描述
        const char* CurrentFirmwareVersion; // 当前固件版本
        const char* NewFirmwareVersion;     // 最新固件版本
    };

    /*
     ********************************************************************* 请求设备升级固件
     */
    struct ivcpUpdateDeviceVersionInfo
    {
        const char* deviceID;           // 设备ID
        const char* requestID;          // 请求ID
        const char* NewFirmwareVersion; // 最新固件版本
    };
    struct ivcpUpdateDeviceVersionRspInfo
    {
        int code;        // 错误码
        const char* msg; // 错误描述
    };

    /*
    ********************************************************************* 请求推流上云
    */
    struct ivcpPullStreamToCloudMsg
    {
        const char* deviceID;  // 设备ID
        const char* requestID; // 请求ID
        const char* action;    // 动作，open，close
    };
    struct ivcpPullStreamToCloudRspMsg
    {
        int code;        // 视频流回调任务创建情况
        const char* msg; // 错误描述
    };

    /*
     ********************************************************************* 请求推流上cos
     */
    struct ivcpPullStreamToCosMsg
    {
        const char* deviceID;  // 设备ID
        const char* requestID; // 请求ID
        const char* action;    // 动作，open，close
        const char* StreamId;  // 通道抽象ID
    };
    struct ivcpPullStreamToCosRspMsg
    {
        int code;        // 视频流回调任务创建情况
        const char* msg; // 错误描述
    };

    /*
     ********************************************************************* 请求推流P2P
     */
    struct ivcpPullStreamToP2PMsg
    {
        const char* deviceID;  // 设备ID
        const char* requestID; // 请求ID
        const char* PlayerID;  // 播放器ID
        int ChannelID;         // 通道ID
        const char* action;    // 动作，open，close
        const char* PlayerIp;
        const char* DeviceIp;
        int PlayerPort;
        int DevicePort;
    };
    struct ivcpPullStreamToP2PRspMsg
    {
        int code;        // 视频流回调任务创建情况
        const char* msg; // 错误描述
    };

    /*
     ********************************************************************* 设备上传图片时，图片的详细信息
     */
    struct ivcpImageMetaData
    {
        const char* DeviceID;   // 设备ID
        int ChannelID;          // 通道ID
        const char* VideoID;    // 图片关联的视频片段id，非必须
        const char* FileFormat; // 图片格式
        long long ShotTime;     // 图片生成日期(unix 时间戳)
        const char* Title;      // 图片名称
        const char* EventType;  // 图片由于什么事件生成的
        int Height;             // 图片高度，单位，像素
        int Width;              // 图片宽度，单位，像素
        int FileSize;           // 图片大小，单位，KB
    };
    /*
     ********************************************************************* 设备上传视频片段时，片段的详细信息
     */
    struct ivcpVideoSliceMetaData
    {
        const char* DeviceID; // 设备ID
        int ChannelID;          // 通道ID
        const char* FileFormat; // 图片格式
        long long BeginTime;    // 开始日期(unix 时间戳)
        long long EndTime;      // 开始日期(unix 时间戳)
        const char* Title;      // 视频名称名称
        const char* EventType;  // 视频由于什么事件生成的
        int Height;             // 图片高度，单位，像素
        int Width;              // 图片宽度，单位，像素
        int FileSize;           // 视频大小，单位，KB
    };

    /*
    ********************************************************************设备上传视频片段时,片段的音视频参数
    */
    struct ivcpVideoSliceAvParam
    {
        const char* DeviceID; // 设备ID

        int audiocodecid;
        double audiodatarate; // kbps
        int audiosamplerate;
        int audiosamplesize;
        int stereo;

        int videocodecid;
        double videodatarate; // kbps
        double framerate;     // fps
        double duration;
        int interval; // frame interval
        int width;
        int height;
    };
    /*
     ********************************************************************音视频元数据
     */
    typedef struct VideoMetadata
    {
        double nWidth;        // 视频宽
        double nHeight;       // 视频高
        double nFrameRate;    // fps

        unsigned char* Sps;
        int nSpsLen;

        unsigned char* Pps;
        int nPpsLen;

        unsigned char* Vps;
        int nVpsLen;

    } VideoMetadata, * LPVideoMetadata;

    typedef struct AudioMetadata
    {
        // audio
        int hasAudioMetaData;           // 是否含有音频元信息
        double nAudioFormat;               // 音频编码类型 7 = 711A 8= 711u 10 = aac 
        unsigned int nIsStereo;         // 是否立体声 0，否， 1，是
        double nSamplerate;               // 音频采样率

        //audio dec
        int hasAudioSpecCfg;            //是否含有音频解码参数
        unsigned char* pAudioSpecCfg;   // 音频解码参数
        unsigned int nAudioSpecCfgLen;  // 音频解码参数长度，一般为2

    } AudioMetadata, * LPAudioMetadata;

#ifdef __cplusplus
}
#endif
#endif