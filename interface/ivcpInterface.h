#ifndef _IVCP_INTERFACE_H__
#define _IVCP_INTERFACE_H__

#include "ivcpMsgStruct.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif
   /************************************************* 
   Function:       //  IvcpInitIvcpSdk
   Description:    //  SDK初始化
   Input:          //  deviceSN：设备的序列号    
                   //  p：调用者自己的回调函数 ，形如 void func(const void*, int) 
   Output:         //  无
   Return:         //  返回0，初始化成功，返回其他，初始化失败，错误码详情，见ivcpMsgType.h
   Others:         //  在程序开始时调用
   *************************************************/
   extern int IvcpInitIvcpSdk(const char* deviceSN, void (*p)(const void*, int));

   /************************************************* 
   Function:       //  IvcpUnInitIvcpSdk
   Description:    //  SDK反初始化
   Input:          //  无
   Output:         //  无
   Return:         //  返回0，反初始化成功，返回其他，反初始化失败，错误码详情，见ivcpMsgType.h
   Others:         //  在程序退出时调用
   *************************************************/
   extern int IvcpUnInitIvcpSdk();

   /************************************************* 
   Function:       //  IvcpUploadImageData
   Description:    //  上传图片数据
   Input:          //  imageID 图像ID，保证全局唯一
                   //  imageData： 图像数据
                   //  imageDataLen： ImageData长度
                   //  imageMetaData： 图像详细数据
   Output:         //  无
   Return:         //  返回0，成功，返回其他，失败，错误码详情，见ivcpMsgType.h
   Others:         //  无
   *************************************************/
   extern int IvcpUploadImageData(const char* imageID, const unsigned char* imageData, int imageDataLen, const ivcpImageMetaData* imageMetaData);

   /************************************************* 
      Function:       //  IvcpUploadVideoSliceData
      Description:    //  上传视频片段数据，VideoSliceData为已封装好的视频文件（包括视频和音频），比如 test.mp4
      Input:          //  videoSliceID：视频ID，保证全局唯一   
                      //  videoSliceData：视频片段数据
                      //  videoSliceDataLen：videoSliceDataLen长度
                      //  videoSliceMetaData：视频片段详细数据
      Output:         //  无
      Return:         //  返回0，成功，返回其他，失败，错误码详情，见ivcpMsgType.h
      Others:         //  无
   *************************************************/
   extern int IvcpUploadVideoSliceData(const char* videoSliceID, const unsigned char* videoSliceData, int videoSliceDataLen, const ivcpVideoSliceMetaData* videoSliceMetaData);

   /************************************************* 
   Function:       //  IvcpAudioVideoData
   Description:    //  回调音视频流
   Input:          
                   //  isMetaData:当前回调是否为metadata, 定义为：0（否）、1（是）
                   //  channelID:当前回调数据的通道ID
                   //  data:音视频数据
                   //  dataLen:音视频数据长度
                   //  dataType:音视频数据类型，定义为：1(h264)、2(h265)、3(aac)、4(g.711u)、4(g.711a)
                   //  pts： 显示时间戳
                   //  LPVideoMetadata: 设备视频的metadata
                   //  LPAudioMetadata: 设备视频的metadata
   Output:         //  无
   Return:         //  返回0，成功，返回其他，失败，错误码详情，见ivcpMsgType.h
   Others:         //  无
   *************************************************/
   extern int IvcpAudioVideoData(int isMetaData, int channelID, const unsigned char* data, int dataLen,
      int dataType, unsigned int dts, unsigned int pts,
      const LPVideoMetadata videoMetaData = NULL, const LPAudioMetadata audioMetaData = NULL);

   /************************************************* 
   Function:       //  IvcpCommonMsg
   Description:    //  用于调用者跟SDK普通交互， 
   Input:          //  requstID： 请求ID
                   //  msgType： 请求类型
                   //  msgStruct：请求数据详情，由调用者创建和释放入参，接口只负责写入数据
   Output:         //  无
   Return:         //  返回0，与云服务器通讯成功，返回其他，与云服务器通讯失败，错误码详情，见ivcpMsgType.h
   Others:         //  除了上传图片数据和音视频数据，其他程序和SDK之间的请求响应均通过此接口
                       注：接口不负责入参资源的释放
   *************************************************/
   extern int IvcpCommonMsg(const char* requstID, int msgType, const void* msgStruct);

#ifdef __cplusplus
};
#endif

#endif
