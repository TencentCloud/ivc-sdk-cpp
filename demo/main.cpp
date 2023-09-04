#include <iostream>
#include <memory>
#include <unistd.h>
#include <string>
#include "ivcpInterface.h"
#include "ivcpMsgType.h"
#include "ivcpMsgStruct.h"
#include <thread>
#include <time.h>
#include <cstring>
#include "flv-reader.h"
#include "flv-proto.h"
#include "help.h"
#include "flv-demuxer.h"

std::string deviceSN = "123"; // sts
// std::string deviceSN = "1234"; // ssm
#define FILEBUFSIZE (1024 * 1024 * 10) //  10M

// 此函数禁止阻塞执行
void Callback(const void* msg, int msgType)
{
  switch (msgType)
  {
  case IvcpRequsetDeviceBaseInfo:
  {
    std::cout << "ivcpGetDeviceBaseInfo" << std::endl;
    ivcpGetDeviceBaseInfo* reqInfo = (ivcpGetDeviceBaseInfo*)(msg);

    ivcpGetDeviceBaseRspInfo rspInfo;

    rspInfo.code = Success;
    rspInfo.msg = "";
    rspInfo.DeviceModel = "DH-IPC-HFW4433K-I6";
    rspInfo.Manufacturer = "大华";
    rspInfo.FirmwareVersion = "v1.0.0.1";
    rspInfo.DeviceName = "新长安广场800楼门口";
    rspInfo.ProtocolType = "ivcp";

    int ret = IvcpCommonMsg(reqInfo->requestID, msgType, &rspInfo);
  }
  case IvcpRequsetDeviceChannelInfo:
  {
    std::cout << "IvcpRequsetDeviceChannelInfo" << std::endl;
    ivcpGetDeviceChannelInfo* reqInfo = (ivcpGetDeviceChannelInfo*)(msg);

    ivcpGetDeviceChannelRspInfo rspInfo;

    ivcpChannelInfo channelInfo;
    channelInfo.channelID = 1;

    channelInfo.channelName = "摄像头1";
    channelInfo.channelType = 1;
    channelInfo.channelState = 1;

    rspInfo.code = Success;
    rspInfo.msg = "";
    rspInfo.channelNum = 1; // ipc只有一个通道

    rspInfo.channelInfos[0].channelID = channelInfo.channelID;
    rspInfo.channelInfos[0].channelName = channelInfo.channelName;
    rspInfo.channelInfos[0].channelType = channelInfo.channelType;
    rspInfo.channelInfos[0].channelState = channelInfo.channelState;

    int ret = IvcpCommonMsg(reqInfo->requestID, msgType, &rspInfo);
  }
  case IvcpRequsetDeviceAudioVideoInfo:
  {
    std::cout << "IvcpRequsetDeviceAudioVideoInfo" << std::endl;
    ivcpGetDeviceAudioVideoInfo* reqInfo = (ivcpGetDeviceAudioVideoInfo*)(msg);

    ivcpGetDeviceAudioVideoRspInfo rspInfo;

    rspInfo.code = Success;
    rspInfo.msg = "";

    rspInfo.channelNum = 1;

    rspInfo.AvInfos[0].VideoCodecType = "h264";
    rspInfo.AvInfos[0].AudioCodecType = "aac";
    rspInfo.AvInfos[0].StreamType = "first";
    rspInfo.AvInfos[0].ComplexType = "complex";
    rspInfo.AvInfos[0].CodeRate = 1024;

    rspInfo.AvInfos[0].CodeType = "constant";
    rspInfo.AvInfos[0].Height = 1080;
    rspInfo.AvInfos[0].Width = 1920;
    rspInfo.AvInfos[0].Fps = -1;
    rspInfo.AvInfos[0].Quality = 5;
    rspInfo.AvInfos[0].Gop = 50;

    int ret = IvcpCommonMsg(reqInfo->requestID, msgType, &rspInfo);
  }
  case IvcpRequsetDeviceRecordInfo:
  {
    std::cout << "IvcpRequsetDeviceRecordInfo" << std::endl;
    ivcpGetDeviceRecordInfo* reqInfo = (ivcpGetDeviceRecordInfo*)(msg);

    ivcpGetDeviceRecordRspInfo rspInfo;

    rspInfo.code = Success;
    rspInfo.msg = "";
    rspInfo.channelNum = 1;
    rspInfo.RecordInfos[0].ImageNum = 3;
    rspInfo.RecordInfos[0].VideoDuration = 15;
    rspInfo.RecordInfos[0].ImageOnly = 2;

    int ret = IvcpCommonMsg(reqInfo->requestID, msgType, &rspInfo);
  }
  break;
  case IvcpModifyDeviceBaseInfo:
  {
    std::cout << "IvcpModifyDeviceBaseInfo" << std::endl;
    ivcpModifyDeviceBaseInfo* reqInfo = (ivcpModifyDeviceBaseInfo*)(msg);

    // 修改参数代码

    ivcpModifyDeviceBaseRspInfo rspInfo;

    rspInfo.code = Success;
    rspInfo.msg = "";

    int ret = IvcpCommonMsg(reqInfo->requestID, msgType, &rspInfo);
  }
  break;
  case IvcpModifyDeviceAudioVideoInfo:
  {
    std::cout << "IvcpModifyDeviceAudioVideoInfo" << std::endl;
    ivcpModifyDeviceAudioVideoInfo* reqInfo = (ivcpModifyDeviceAudioVideoInfo*)(msg);

    // 修改参数代码

    ivcpModifyDeviceAudioVideoRspInfo rspInfo;

    rspInfo.code = Success;
    rspInfo.msg = "";

    int ret = IvcpCommonMsg(reqInfo->requestID, msgType, &rspInfo);
  }
  break;
  case IvcpModifyDeviceRecordInfo:
  {
    std::cout << "IvcpModifyDeviceRecordInfo" << std::endl;
    ivcpModifyDeviceRecordInfo* reqInfo = (ivcpModifyDeviceRecordInfo*)(msg);

    // 修改参数代码

    ivcpModifyDeviceRecordRspInfo rspInfo;

    rspInfo.code = Success;
    rspInfo.msg = "";

    int ret = IvcpCommonMsg(reqInfo->requestID, msgType, &rspInfo);
  }
  break;
  case IvcpRequsetGetDeviceVersionInfo:
  {
    std::cout << "IvcpRequsetGetDeviceVersionInfo" << std::endl;

    ivcpGetDeviceVersionInfo* reqInfo = (ivcpGetDeviceVersionInfo*)(msg);

    ivcpGetDeviceVersionRspInfo rspInfo;

    rspInfo.code = Success;
    rspInfo.msg = "";
    rspInfo.CurrentFirmwareVersion = "v1.0.0.1";
    rspInfo.NewFirmwareVersion = "v1.0.0.3";

    int ret = IvcpCommonMsg(reqInfo->requestID, msgType, &rspInfo);
  }
  break;
  case IvcpRequsetUpdateDeviceVersionInfo:
  {
    std::cout << "IvcpRequsetUpdateDeviceVersionInfo" << std::endl;

    ivcpUpdateDeviceVersionInfo* reqInfo = (ivcpUpdateDeviceVersionInfo*)(msg);
    ivcpUpdateDeviceVersionRspInfo rspInfo;

    rspInfo.code = Success;
    rspInfo.msg = "";
    int ret = IvcpCommonMsg(reqInfo->requestID, msgType, &rspInfo);

    // 异步执行固件升级程序
  }
  break;
  default:
    break;
  }
}

void callbackAvData()
{
  int r, type;
  int avcrecord = 0;
  int aacconfig = 0;
  size_t taglen;
  uint32_t timestamp;
  uint32_t s_timestamp = 0;
  uint32_t diff = 0;
  uint64_t clock;
  static char packet[2 * 1024 * 1024]; //读出来的数据不带flv tag
  int tagIndex = 0;
  while (1)
  {
    tagIndex = 0;
    struct flv_reader_t* flvReader = (flv_reader_t*)flv_reader_create("../../testfile/mhls.flv");
    struct flv_demuxer_t* demuxer = flv_demuxer_create(flv_handler, nullptr);
    clock = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); // timestamp start from 0

    while (1 == flv_reader_read(flvReader, &type, &timestamp, &taglen, packet, sizeof(packet)))  //tag size && tag data (no tag header)
    {
      //std::cout << "----------------------------- tag index:" << tagIndex++ << std::endl;
      uint64_t t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();;
      if (clock + timestamp > t && clock + timestamp < t + 3 * 1000) // dts skip
        std::this_thread::sleep_for(std::chrono::milliseconds(clock + timestamp - t));
      else if (clock + timestamp > t + 3 * 1000)
        clock = t - timestamp;

      timestamp += diff;
      s_timestamp = timestamp > s_timestamp ? timestamp : s_timestamp;

      if (FLV_TYPE_AUDIO == type)
      {
        flv_demuxer_input(demuxer, type, packet, taglen, timestamp);

      }
      else if (FLV_TYPE_VIDEO == type)
      {
        flv_demuxer_input(demuxer, type, packet, taglen, timestamp);
      }
      else if (FLV_TYPE_SCRIPT == type)
      {
        VideoMetadata vMetadata;
        AudioMetadata aMetaData;

        aMetaData.hasAudioMetaData = 1;
        aMetaData.hasAudioSpecCfg = 0;

        readFLVScriptData(packet, taglen, vMetadata, aMetaData);
        IvcpAudioVideoData(1, 1, nullptr, 0, -1, 0, 0, &vMetadata, &aMetaData);
      }
      else
      {
        continue;
      }

    }

    flv_reader_destroy(flvReader);

    diff = s_timestamp + 30;
  }
}

void testUploadImageData()
{
  FILE* fp = fopen("../../testfile/test.jpg", "rb");

  fseek(fp, 0, SEEK_SET);
  unsigned char* imagebuf = new unsigned char[FILEBUFSIZE];
  int bufsize = fread(imagebuf, sizeof(unsigned char), FILEBUFSIZE, fp);

  fclose(fp);
  time_t result = time(NULL);
  std::string ImageId = "image" + std::to_string(result);
  ivcpImageMetaData metadata;
  metadata.DeviceID = deviceSN.c_str();
  metadata.ChannelID = 1;
  metadata.VideoID = "123VideoID";
  metadata.FileFormat = "jpg";
  metadata.ShotTime = result;
  metadata.Title = "19楼门口有人员经过抓拍";
  metadata.EventType = "AI行人识别";
  metadata.Height = 698;
  metadata.Width = 1080;
  metadata.FileSize = 98;
  metadata.FileSize = bufsize / 1024;

  sleep(50); // 刚开始没和服务器建立连接，得等一会

  IvcpUploadImageData(ImageId.c_str(), imagebuf, bufsize, &metadata);
}

void testUploadVideoSliceData()
{
  FILE* fp = fopen("../../testfile/vod.mp4", "rb");

  fseek(fp, 0, SEEK_SET);
  unsigned char* buf = new unsigned char[FILEBUFSIZE];
  int bufsize = fread(buf, sizeof(unsigned char), FILEBUFSIZE, fp);

  fclose(fp);
  time_t result = time(NULL);
  std::string VideoSliceId = "video" + std::to_string(result);
  ivcpVideoSliceMetaData metadata;
  metadata.DeviceID = deviceSN.c_str();
  metadata.ChannelID = 1;
  metadata.FileFormat = "mp4";
  metadata.BeginTime = result;
  metadata.EndTime = result + 19;
  metadata.Title = "19楼门口有人员经过19秒视频";
  metadata.EventType = "AI行人识别";
  metadata.Height = 1080;
  metadata.Width = 1920;
  metadata.FileSize = bufsize / 1024;

  sleep(50); // 刚开始没和服务器建立连接，得等一会

  IvcpUploadVideoSliceData(VideoSliceId.c_str(), buf, bufsize, &metadata);
}

int main(int argc, char** argv)
{
  int ret = IvcpInitIvcpSdk(deviceSN.c_str(), Callback);

  if (ret != Success)
    return 0;

  std::thread t1(callbackAvData); // sdk初始化成功后，设备即开始向sdk回调音视频数据，直到主进程退出为止


  while (1)
  {
    std::cout << "------------input num to test " << std::endl;
    std::cout << "1 to test uploadImage" << std::endl;
    std::cout << "2 to test uploadvideoSlice" << std::endl;
    std::cout << "------------100 to exit" << std::endl;
    int num = -1;
    std::cin >> num;
    switch (num)
    {
    case 1:
    {
      std::cout << "********** uploadImage " << std::endl;
      testUploadImageData();
      continue;
    }

    case 2:
    {
      std::cout << "********** testUploadVideoSliceData " << std::endl;
      testUploadVideoSliceData();
      continue;
    }
    case 100:
    {
      std::cout << "********** exit " << std::endl;
    }
    break;
    default:
      continue;
    }
  }
  t1.join();
  IvcpUnInitIvcpSdk();

  return 0;
}
