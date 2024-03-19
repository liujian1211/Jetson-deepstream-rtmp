#include "deepstream_test.h"
#include  <iostream>
#include <fstream>
#include <map>
#include <glib.h>
#include <string>
#include <ctime>
// #include "messageGenerate.h"
#include "tcp_ip_message.h"
#include "getGPS.h"

#define MAX_DISPLAY_LEN 128

// #define PGIE_CLASS_ID_VEHICLE 0
// #define PGIE_CLASS_ID_PERSON 2
#define LABEL_FILE "/home/nano/liujian/test_websocket/utils/labels_roadDamage.txt"


// messageGenerate::messageGenerate(){}
// messageGenerate::~messageGenerate(){}
// messageGenerate MG; //实例化 生成报文类
// #define LOCAL_VIDEO "/opt/nvidia/deepstream/deepstream-5.0/samples/streams/sample_720p.h264"


/* The muxer output resolution must be set if the input streams will be of
 * different resolution. The muxer will scale all the input frames to this
 * resolution. */
#define MUXER_OUTPUT_WIDTH 1920
#define MUXER_OUTPUT_HEIGHT 1080 

/* Muxer batch formation timeout, for e.g. 40 millisec. Should ideally be set
 * based on the fastest source's framerate. */
#define MUXER_BATCH_TIMEOUT_USEC 40000

// tracking 配置文件
#define TRACKER_CONFIG_FILE "/home/nano/liujian/test_websocket/utils/tracker_config.txt"
#define MAX_TRACKING_ID_LEN 16

//定义track配置文件内的变量
#define CONFIG_GROUP_TRACKER "tracker"
#define CONFIG_GROUP_TRACKER_WIDTH "tracker-width"
#define CONFIG_GROUP_TRACKER_HEIGHT "tracker-height"
#define CONFIG_GROUP_TRACKER_LL_CONFIG_FILE "ll-config-file"
#define CONFIG_GROUP_TRACKER_LL_LIB_FILE "ll-lib-file"
#define CONFIG_GROUP_TRACKER_ENABLE_BATCH_PROCESS "enable-batch-process"
#define CONFIG_GROUP_TRACKER_ENABLE_PAST_FRAME "enable-past-frame"
#define CONFIG_GPU_ID "gpu-id"
#define CONFIG_DISPLAY_TRACKING_ID "display-tracking-id"

// 需要显示的经纬度
static std::string lat;
static std::string lon;

std::vector<guint64>  DeepStream::g_Transverse_cracks_ids;
std::vector<guint64>  DeepStream::g_Linear_cracks_ids;
std::vector<guint64>  DeepStream::g_Pit_slot_ids;
std::vector<guint64>  DeepStream::g_Crack_ids;
std::vector<guint64>  DeepStream::g_Mark_ids;
std::vector<guint64>  DeepStream::g_Tyreskidmark_ids;
std::vector<guint64>  DeepStream::g_patched_ids;
std::vector<guint64>  DeepStream::g_manhole_ids;
std::vector<guint64>  DeepStream::g_joint_ids;
std::vector<guint64>  DeepStream::g_trash_ids;
std::vector<guint64>  DeepStream::g_puddle_ids;
std::vector<guint64>  DeepStream::g_repaired_crack_ids;
std::vector<guint64>  DeepStream::g_animal_ids;
std::vector<guint64>  DeepStream::g_shoes_ids;
std::vector<guint64>  DeepStream::g_bumps_ids;
std::vector<guint64>  DeepStream::g_shadow_ids;
std::vector<guint64>  DeepStream::g_incomplete_mark_ids;
std::vector<guint64>  DeepStream::g_crushing_plate_ids;
std::vector<guint64>  DeepStream::g_faulting_ids; 

/* osd_sink_pad_buffer_probe  will extract metadata received on OSD sink pad
 * and update params for drawing rectangle, object information etc. */
void DeepStream::data(std::string data1,std::string data2)
{       
    lat = data1;
    lon = data2;  
    // std::cout<<"date里的lat的值为"<<lat<<std::endl;
    // std::cout<<"date里的lon的值为"<<lon<<std::endl;
}


static GstPadProbeReturn osd_sink_pad_buffer_probe (GstPad * pad, GstPadProbeInfo * info, gpointer u_data)
{     
    
    GstBuffer *buf = (GstBuffer *) info->data;

    NvDsObjectMeta *obj_meta = NULL;

    NvDsMetaList * l_frame = NULL;
    NvDsMetaList * l_obj = NULL;
    NvDsDisplayMeta *display_meta = NULL;

    //获取批处理元数据
    NvDsBatchMeta *batch_meta = gst_buffer_get_nvds_batch_meta (buf);

    // std::cout<<"lat的值为"<<lat<<std::endl;
    // std::cout<<"lon的值为"<<lon<<std::endl;

    //遍历批处理元数据，得到每一帧的元数据
    for (l_frame = batch_meta->frame_meta_list; l_frame != NULL; l_frame = l_frame->next)
    {
        //获取每一帧的元数据
        NvDsFrameMeta *frame_meta = (NvDsFrameMeta *) (l_frame->data);
        int offset = 0;

        //遍历每一帧的元数据，得到每一个检测到的物体的元数据
        for (l_obj = frame_meta->obj_meta_list; l_obj != NULL;l_obj = l_obj->next) 
        {
          obj_meta = (NvDsObjectMeta *) (l_obj->data);
          
          if (obj_meta->class_id == 0) 
          {
            // 如果object id不在g_Transverse_cracks_ids中，添加进去
            if (std::find(DeepStream::g_Transverse_cracks_ids.begin(), DeepStream::g_Transverse_cracks_ids.end(), obj_meta->object_id) == DeepStream::g_Transverse_cracks_ids.end())
            {                                
              DeepStream::g_Transverse_cracks_ids.push_back(obj_meta->object_id);
            }              
            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=0;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0;                            
          }
          if (obj_meta->class_id == 1) 
          {
            if (std::find(DeepStream::g_Linear_cracks_ids.begin(), DeepStream::g_Linear_cracks_ids.end(), obj_meta->object_id) == DeepStream::g_Linear_cracks_ids.end())
            {
              DeepStream::g_Linear_cracks_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0;    
          }
          if(obj_meta->class_id == 2)
          {
            if (std::find(DeepStream::g_Pit_slot_ids.begin(), DeepStream::g_Pit_slot_ids.end(), obj_meta->object_id) == DeepStream::g_Pit_slot_ids.end())
            {
              DeepStream::g_Pit_slot_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 3)
          {
            if (std::find(DeepStream::g_Crack_ids.begin(), DeepStream::g_Crack_ids.end(), obj_meta->object_id) == DeepStream::g_Crack_ids.end())
            {
              DeepStream::g_Crack_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 4)
          {
            if (std::find(DeepStream::g_Mark_ids.begin(), DeepStream::g_Mark_ids.end(), obj_meta->object_id) == DeepStream::g_Mark_ids.end())
            {
              DeepStream::g_Mark_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 5)
          {
            if (std::find(DeepStream::g_Tyreskidmark_ids.begin(), DeepStream::g_Tyreskidmark_ids.end(), obj_meta->object_id) == DeepStream::g_Tyreskidmark_ids.end())
            {
              DeepStream::g_Tyreskidmark_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 6)
          {
            if (std::find(DeepStream::g_patched_ids.begin(), DeepStream::g_patched_ids.end(), obj_meta->object_id) == DeepStream::g_patched_ids.end())
            {
              DeepStream::g_patched_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 7)
          {
            if (std::find(DeepStream::g_manhole_ids.begin(), DeepStream::g_manhole_ids.end(), obj_meta->object_id) == DeepStream::g_manhole_ids.end())
            {
              DeepStream::g_manhole_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 8)
          {
            if (std::find(DeepStream::g_joint_ids.begin(), DeepStream::g_joint_ids.end(), obj_meta->object_id) == DeepStream::g_joint_ids.end())
            {
              DeepStream::g_joint_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 9)
          {
            if (std::find(DeepStream::g_trash_ids.begin(), DeepStream::g_trash_ids.end(), obj_meta->object_id) == DeepStream::g_trash_ids.end())
            {
              DeepStream::g_trash_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 10)
          {
            if (std::find(DeepStream::g_puddle_ids.begin(), DeepStream::g_puddle_ids.end(), obj_meta->object_id) == DeepStream::g_puddle_ids.end())
            {
              DeepStream::g_puddle_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 11)
          {
            if (std::find(DeepStream::g_repaired_crack_ids.begin(), DeepStream::g_repaired_crack_ids.end(), obj_meta->object_id) == DeepStream::g_repaired_crack_ids.end())
            {
              DeepStream::g_repaired_crack_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 12)
          {
            if (std::find(DeepStream::g_animal_ids.begin(), DeepStream::g_animal_ids.end(), obj_meta->object_id) == DeepStream::g_animal_ids.end())
            {
              DeepStream::g_animal_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 13)
          {
            if (std::find(DeepStream::g_shoes_ids.begin(), DeepStream::g_shoes_ids.end(), obj_meta->object_id) == DeepStream::g_shoes_ids.end())
            {
              DeepStream::g_shoes_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 14)
          {
            if (std::find(DeepStream::g_bumps_ids.begin(), DeepStream::g_bumps_ids.end(), obj_meta->object_id) == DeepStream::g_bumps_ids.end())
            {
              DeepStream::g_bumps_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 15)
          {
            if (std::find(DeepStream::g_shadow_ids.begin(), DeepStream::g_shadow_ids.end(), obj_meta->object_id) == DeepStream::g_shadow_ids.end())
            {
              DeepStream::g_shadow_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 16)
          {
            if (std::find(DeepStream::g_incomplete_mark_ids.begin(), DeepStream::g_incomplete_mark_ids.end(), obj_meta->object_id) == DeepStream::g_incomplete_mark_ids.end())
            {
              DeepStream::g_incomplete_mark_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 17)
          {
            if (std::find(DeepStream::g_crushing_plate_ids.begin(), DeepStream::g_crushing_plate_ids.end(), obj_meta->object_id) == DeepStream::g_crushing_plate_ids.end())
            {
              DeepStream::g_crushing_plate_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
          if(obj_meta->class_id == 18)
          {
            if (std::find(DeepStream::g_faulting_ids.begin(), DeepStream::g_faulting_ids.end(), obj_meta->object_id) == DeepStream::g_faulting_ids.end())
            {
              DeepStream::g_faulting_ids.push_back(obj_meta->object_id);                      
            }

            obj_meta->text_params.font_params.font_size=15;
            obj_meta->text_params.font_params.font_color.red=255;
            obj_meta->text_params.font_params.font_color.green = 255;
            obj_meta->text_params.font_params.font_color.blue = 0;
            obj_meta->text_params.font_params.font_color.alpha = 1.0; 
          }
        }
   

        //获取显示元数据
        display_meta = nvds_acquire_display_meta_from_pool(batch_meta);
        
        //添加识别对象的显示文字
        NvOSD_TextParams *txt_params  = &display_meta->text_params[0];
        display_meta->num_labels = 4; //显示多少段文字就要改成多少
        txt_params->display_text = (char*)g_malloc0 (MAX_DISPLAY_LEN);
        offset = snprintf(txt_params->display_text, MAX_DISPLAY_LEN, "横向裂缝:%d  ", DeepStream::g_Transverse_cracks_ids.size());
        offset += snprintf(txt_params->display_text+offset, MAX_DISPLAY_LEN-offset, "纵向裂缝:%d  ", DeepStream::g_Linear_cracks_ids.size());
        offset += snprintf(txt_params->display_text+offset, MAX_DISPLAY_LEN-offset, "坑槽:%d  ", DeepStream::g_Pit_slot_ids.size());
        offset += snprintf(txt_params->display_text+offset, MAX_DISPLAY_LEN-offset, "龟裂:%d  ", DeepStream::g_Crack_ids.size());
        offset += snprintf(txt_params->display_text+offset, MAX_DISPLAY_LEN-offset, "抛洒物:%d  ", DeepStream::g_trash_ids.size());
        offset += snprintf(txt_params->display_text+offset, MAX_DISPLAY_LEN-offset, "标线缺失:%d  ", DeepStream::g_incomplete_mark_ids.size());
        offset += snprintf(txt_params->display_text+offset, MAX_DISPLAY_LEN-offset, "错台:%d  ", DeepStream::g_faulting_ids.size());
        offset += snprintf(txt_params->display_text+offset, MAX_DISPLAY_LEN-offset, "破碎板:%d  ", DeepStream::g_crushing_plate_ids.size());

        // 添加时间显示文字
        NvOSD_TextParams *txt_time  = &display_meta->text_params[1];
        txt_time->display_text = (char*)g_malloc0(MAX_DISPLAY_LEN);
       
        time_t current_time = time(nullptr);
        struct tm *localTime = localtime(&current_time);
        std::stringstream ss;
        ss<<localTime->tm_year +1900<<"-"<<std::setw(2)<<std::setfill('0')<<localTime->tm_mon+1<<"-";
        ss<<std::setw(2)<<std::setfill('0')<<localTime->tm_mday<<" ";
        ss << std::setw(2) << std::setfill('0') << localTime->tm_hour << ":";
        ss << std::setw(2) << std::setfill('0') << localTime->tm_min << ":";
        ss << std::setw(2) << std::setfill('0') << localTime->tm_sec;
        std::string time_display = ss.str();
        snprintf(txt_time->display_text ,MAX_DISPLAY_LEN, time_display.c_str());
        
        //添加车牌显示文字
        NvOSD_TextParams *txt_carNum = &display_meta->text_params[2];
        txt_carNum->display_text = (char*)g_malloc0(MAX_DISPLAY_LEN);
        
        std::string carNum_display = "苏DDP2058";
        snprintf(txt_carNum->display_text,MAX_DISPLAY_LEN,carNum_display.c_str());

        // 添加经纬度显示文字
        NvOSD_TextParams *txt_location = &display_meta->text_params[3];
        txt_location->display_text = (char*)g_malloc0(MAX_DISPLAY_LEN);

        std::string location_display =lat+" "+lon;
        snprintf(txt_location->display_text,MAX_DISPLAY_LEN,location_display.c_str());

        /* 设置识别对象文字的位置 */
        txt_params->x_offset = 10;
        txt_params->y_offset = 52;

        /* 设置时间显示文字的位置 */
        txt_time->x_offset = 10;
        txt_time->y_offset = 12;

        /* 设置车牌号显示文字的位置 */ 
        txt_carNum->x_offset = 10;
        txt_carNum->y_offset = 92;

        /* 设置经纬度显示文字的位置 */
        txt_location->x_offset = 10;
        txt_location->y_offset = 132;

        /* 识别对象文字的字体 , 字体颜色 和 字体尺寸 */
        txt_params->font_params.font_name = "SimSun";
        txt_params->font_params.font_size = 15;
        txt_params->font_params.font_color.red = 1.0;
        txt_params->font_params.font_color.green = 1.0;
        txt_params->font_params.font_color.blue = 1.0;
        txt_params->font_params.font_color.alpha = 1.0;

        /* 时间显示文字的字体 , 字体颜色 和 字体尺寸 */
        txt_time->font_params.font_name = "Serif";
        txt_time->font_params.font_size = 15;
        txt_time->font_params.font_color.red = 1.0;
        txt_time->font_params.font_color.green = 1.0;
        txt_time->font_params.font_color.blue = 1.0;
        txt_time->font_params.font_color.alpha = 1.0;

        /* 车牌显示文字的字体 , 字体颜色 和 字体尺寸 */ 
        txt_carNum->font_params.font_name = "SimSun"; //SimSun字体支持显示中文
        txt_carNum->font_params.font_size = 15;
        txt_carNum->font_params.font_color.red = 1.0;
        txt_carNum->font_params.font_color.green = 1.0;
        txt_carNum->font_params.font_color.blue = 1.0;
        txt_carNum->font_params.font_color.alpha = 1.0;

        /* 经纬度显示文字的字体 , 字体颜色 和 字体尺寸 */
        txt_location->font_params.font_name = "SimSun"; //SimSun字体支持显示中文
        txt_location->font_params.font_size = 15;
        txt_location->font_params.font_color.red = 1.0;
        txt_location->font_params.font_color.green = 1.0;
        txt_location->font_params.font_color.blue = 1.0;
        txt_location->font_params.font_color.alpha = 1.0;


        /* 识别对象文字的背景颜色 */
        txt_params->set_bg_clr = 1;
        txt_params->text_bg_clr.red = 0.0;
        txt_params->text_bg_clr.green = 0.0;
        txt_params->text_bg_clr.blue = 0.0;
        txt_params->text_bg_clr.alpha = 1.0;

        /* 时间显示文字的背景颜色 */
        txt_time->set_bg_clr = 1;
        txt_time->text_bg_clr.red = 0.0;
        txt_time->text_bg_clr.green = 0.0;
        txt_time->text_bg_clr.blue = 0.0;
        txt_time->text_bg_clr.alpha = 1.0;

        /* 车牌显示文字的背景颜色 */
        txt_carNum->set_bg_clr = 1;
        txt_carNum->text_bg_clr.red = 0.0;
        txt_carNum->text_bg_clr.green = 0.0;
        txt_carNum->text_bg_clr.blue = 0.0;
        txt_carNum->text_bg_clr.alpha = 1.0;

        /*经纬度显示文字的背景颜色 */
        txt_location->set_bg_clr = 1;
        txt_location->text_bg_clr.red = 0.0;
        txt_location->text_bg_clr.green = 0.0;
        txt_location->text_bg_clr.blue = 0.0;
        txt_location->text_bg_clr.alpha = 1.0;

        //添加显示
        nvds_add_display_meta_to_frame(frame_meta, display_meta);

        
    }

    // g_print ("Frame Number = %d Number of objects = %d "
    //         "Vehicle Count = %d Person Count = %d\n",
    //         frame_number, num_rects, vehicle_count, person_count);
    // frame_number++;
    return GST_PAD_PROBE_OK;
}

static gboolean
bus_call (GstBus * bus, GstMessage * msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop *) data;
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
      g_print ("End of stream\n");
      g_main_loop_quit (loop);
      break;
    case GST_MESSAGE_ERROR:{
      gchar *debug;
      GError *error;
      gst_message_parse_error (msg, &error, &debug);
      g_printerr ("ERROR from element %s: %s\n",
          GST_OBJECT_NAME (msg->src), error->message);
      if (debug)
        g_printerr ("Error details: %s\n", debug);
      g_free (debug);
      g_error_free (error);
      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }
  return TRUE;
}

//解析追踪的配置文件，即tracker_config.txt
#define CHECK_ERROR(error)                                                   \
if (error)                                                               \
{                                                                        \
    g_printerr("Error while parsing config file: %s\n", error->message); \
    goto done;                                                           \
}



//获取文件的绝对路径
static gchar *
get_absolute_file_path(gchar *cfg_file_path, gchar *file_path)
{
   
    gchar abs_cfg_path[PATH_MAX + 1];
    gchar *abs_file_path;
    gchar *delim;

    if (file_path && file_path[0] == '/')
    {
        return file_path;
    }

    if (!realpath(cfg_file_path, abs_cfg_path))
    {
        g_free(file_path);
        return NULL;
    }

    // Return absolute path of config file if file_path is not NULL.
    if (!file_path)
    {
        abs_file_path = g_strdup(abs_cfg_path);
        return abs_file_path;
    }

    delim = g_strrstr(abs_cfg_path, "/");
    *(delim + 1) = '\0';

    abs_file_path = g_strconcat(abs_cfg_path, file_path, NULL);
    g_free(file_path);

    std::cout<<"绝对路径为"<<abs_file_path<<std::endl;
    return abs_file_path;
}

// 从配置文件中读取配置信息， 设置tracker的属性
static gboolean set_tracker_properties(GstElement *nvtracker)
{
    gboolean ret = FALSE;
    GError *error = NULL;
    gchar **keys = NULL;
    gchar **key = NULL;
    GKeyFile *key_file = g_key_file_new();

    if (!g_key_file_load_from_file(key_file, TRACKER_CONFIG_FILE, G_KEY_FILE_NONE,
                                   &error))
    {
        g_printerr("Failed to load config file: %s\n", error->message);
        return FALSE;
    }

    keys = g_key_file_get_keys(key_file, CONFIG_GROUP_TRACKER, NULL, &error);
    CHECK_ERROR(error);

    for (key = keys; *key; key++)
    {
        if (!g_strcmp0(*key, CONFIG_GROUP_TRACKER_WIDTH))
        {
            gint width =
                g_key_file_get_integer(key_file, CONFIG_GROUP_TRACKER,
                                       CONFIG_GROUP_TRACKER_WIDTH, &error);
            CHECK_ERROR(error);
            g_object_set(G_OBJECT(nvtracker), "tracker-width", width, NULL);
        }
        else if (!g_strcmp0(*key, CONFIG_GROUP_TRACKER_HEIGHT))
        {
            gint height =
                g_key_file_get_integer(key_file, CONFIG_GROUP_TRACKER,
                                       CONFIG_GROUP_TRACKER_HEIGHT, &error);
            CHECK_ERROR(error);
            g_object_set(G_OBJECT(nvtracker), "tracker-height", height, NULL);
        }
        else if (!g_strcmp0(*key, CONFIG_GPU_ID))
        {
            guint gpu_id =
                g_key_file_get_integer(key_file, CONFIG_GROUP_TRACKER,
                                       CONFIG_GPU_ID, &error);
            CHECK_ERROR(error);
            g_object_set(G_OBJECT(nvtracker), "gpu_id", gpu_id, NULL);
        }
        else if (!g_strcmp0(*key, CONFIG_DISPLAY_TRACKING_ID))
        {
          guint display_tracking_id = 
            g_key_file_get_integer(key_file ,CONFIG_GROUP_TRACKER,
                                   CONFIG_DISPLAY_TRACKING_ID,&error);
          CHECK_ERROR(error);
          g_object_set(G_OBJECT(nvtracker),"display-tracking-id",display_tracking_id,NULL);
        }
        else if (!g_strcmp0(*key, CONFIG_GROUP_TRACKER_LL_CONFIG_FILE))
        {
            char *ll_config_file = get_absolute_file_path(TRACKER_CONFIG_FILE,
                                                          g_key_file_get_string(key_file,
                                                                                CONFIG_GROUP_TRACKER,
                                                                                CONFIG_GROUP_TRACKER_LL_CONFIG_FILE, &error));
            CHECK_ERROR(error);
            g_object_set(G_OBJECT(nvtracker), "ll-config-file", ll_config_file, NULL);
        }
        else if (!g_strcmp0(*key, CONFIG_GROUP_TRACKER_LL_LIB_FILE))
        {
            char *ll_lib_file = get_absolute_file_path(TRACKER_CONFIG_FILE,
                                                       g_key_file_get_string(key_file,
                                                                             CONFIG_GROUP_TRACKER,
                                                                             CONFIG_GROUP_TRACKER_LL_LIB_FILE, &error));
            CHECK_ERROR(error);
            g_object_set(G_OBJECT(nvtracker), "ll-lib-file", ll_lib_file, NULL);
        }
        else if (!g_strcmp0(*key, CONFIG_GROUP_TRACKER_ENABLE_BATCH_PROCESS))
        {
            gboolean enable_batch_process =
                g_key_file_get_integer(key_file, CONFIG_GROUP_TRACKER,
                                       CONFIG_GROUP_TRACKER_ENABLE_BATCH_PROCESS, &error);
            CHECK_ERROR(error);
            g_object_set(G_OBJECT(nvtracker), "enable_batch_process",
                         enable_batch_process, NULL);
        }
        else if (!g_strcmp0(*key, CONFIG_GROUP_TRACKER_ENABLE_PAST_FRAME))
        {
            gboolean enable_past_frame = 
                g_key_file_get_integer(key_file, CONFIG_GROUP_TRACKER,
                                       CONFIG_GROUP_TRACKER_ENABLE_PAST_FRAME,&error);
            CHECK_ERROR(error);
            g_object_set(G_OBJECT(nvtracker),"enable_past_frame",
                          enable_past_frame,NULL);
        }
        
        else
        {
            g_printerr("Unknown key '%s' for group [%s]", *key,
                       CONFIG_GROUP_TRACKER);
        }
    }

    ret = TRUE;
done:
    if (error)
    {
        g_error_free(error);
    }
    if (keys)
    {
        g_strfreev(keys);
    }
    if (!ret)
    {
        g_printerr("%s failed", __func__);
    }
    return ret;
}


//usb推理在本地显示指令：gst-launch-1.0 v4l2src device=/dev/video0 ! 'video/x-raw, format=YUY2, width=1920, height=1080' ! nvvidconv ! 'video/x-raw(memory:NVMM), format=NV12' ! nvoverlaysink
//上传rtmp指令：gst-launch-1.0 -v v4l2src device=/dev/video0 ! videoconvert ! x264enc ! flvmux ! rtmpsink location="rtmp://server-ip/app/stream-key"
//运行的时候在build路径下的终端输入：PRE_LOAD=../utils/libmyplugins.so ./target
//使用自己的engine文件主要在于改变dstest1_pgie_config.txt

int DeepStream::deepstream_func()
{

  std::stringstream ss;
  messageGenerate MG;
  ss <<"rtmp://112.82.244.90:1935/live/vehicle_" <<MG.deviceNum;
  std::string RTMP_SERVER_URL = ss.str();

  GMainLoop *loop = NULL;
  GstElement *pipeline = NULL, *source = NULL, *h264parser = NULL,
             *decoder = NULL, *streammux = NULL, *sink = NULL, 
             *pgie = NULL, *nvvidconv = NULL, *nvosd = NULL,
             *cap_filter = NULL, *cap_filter1 = NULL , *transform = NULL,
             *nvvidconv1 = NULL,*nvvidconv2 = NULL,*nvtracker = NULL,
             *encoder=NULL , *converter=NULL ,*muxer=NULL;

  GstBus *bus = NULL;
  guint bus_watch_id;
  GstPad *osd_sink_pad = NULL;
  GstCaps *caps = NULL, *caps1 = NULL, *convertCaps = NULL;
  GstCapsFeatures *feature = NULL;

  gst_init(NULL,NULL);
  loop = g_main_loop_new (NULL, FALSE);

  // Create Pipeline element that will form a connection of other elements 
  pipeline = gst_pipeline_new ("dstest1-usb-cam-pipeline");

  /* Source element for reading from camera */
  source = gst_element_factory_make ("v4l2src", "src_elem");

  h264parser = gst_element_factory_make ("h264parse","parser");

  /* capsfilter for v4l2src */
  cap_filter1 = gst_element_factory_make("capsfilter", "src_cap_filter1");

  cap_filter = gst_element_factory_make ("capsfilter", "src_cap_filter");

  nvvidconv1 = gst_element_factory_make ("videoconvert", "nvvidconv1");

  nvvidconv2 = gst_element_factory_make ("nvvideoconvert", "nvvidconv2");

  // Use convertor to convert from NV12 to RGBA as required by nvosd 
  nvvidconv = gst_element_factory_make ("nvvideoconvert", "osd_conv");

  pgie = gst_element_factory_make ("nvinfer", "primary-nvinference-engine");

  //创建追踪元素
  nvtracker = gst_element_factory_make("nvtracker", "tracker");

  // Create OSD to draw on the converted RGBA buffer 
  nvosd = gst_element_factory_make ("nvdsosd", "nv-onscreendisplay");

  transform = gst_element_factory_make ("nvegltransform", "nvegl-transform");

  // sink = gst_element_factory_make ("nveglglessink", "nvvideo-renderer");
  sink = gst_element_factory_make ("rtmpsink","sink");

  encoder = gst_element_factory_make ("nvv4l2h264enc", "encoder");

  muxer = gst_element_factory_make ("flvmux","muxer");

  converter = gst_element_factory_make ("nvvideoconvert", "converter");

  // Create nvstreammux instance to form batches from one or more sources. 
  streammux = gst_element_factory_make ("nvstreammux", "stream-muxer");


  if (!pipeline || !source || !h264parser || !cap_filter1 || !cap_filter 
      || !nvvidconv1 || !nvvidconv2 || !pgie  || !nvvidconv 
      || !nvosd || !transform || !sink || !encoder || !muxer || !converter || !streammux )
  {
    g_printerr ("One or more elements could not be created. Exiting.\n");
    return -1;
  }

  caps1 = gst_caps_from_string ("video/x-raw");
  caps = gst_caps_from_string ("video/x-raw(memory:NVMM), format=NV12, framerate=30/1");

  feature = gst_caps_features_new ("memory:NVMM", NULL);
  gst_caps_set_features (caps, 0, feature);
  g_object_set (G_OBJECT (cap_filter), "caps", caps, NULL);
  g_object_set (G_OBJECT (cap_filter1), "caps", caps1, NULL);

  g_object_set (G_OBJECT (source), "device", "/dev/video0", NULL);

  g_object_set (G_OBJECT (nvvidconv2), "nvbuf-memory-type", 0, NULL);

  g_object_set (G_OBJECT (pgie),"config-file-path", "/home/nano/liujian/test_websocket/utils/dstest1_pgie_config.txt", NULL);

  //设置tracker元素的参数
  set_tracker_properties(nvtracker);

  g_object_set (G_OBJECT (nvvidconv), "gpu-id", 0, NULL);
  g_object_set (G_OBJECT (nvvidconv), "nvbuf-memory-type", 0, NULL);

  g_object_set (G_OBJECT (nvosd), "gpu-id", 0, NULL);

  g_object_set (G_OBJECT (sink), "location", RTMP_SERVER_URL.c_str(), NULL);

  g_object_set (G_OBJECT (streammux), "batch-size", 1, NULL);
  g_object_set (G_OBJECT (streammux), "width", MUXER_OUTPUT_WIDTH,
                                      "height", MUXER_OUTPUT_HEIGHT,
                                      "batched-push-timeout", MUXER_BATCH_TIMEOUT_USEC,
                                      "live-source", TRUE, NULL);    

  gst_bin_add_many (GST_BIN (pipeline), source, cap_filter1,nvvidconv1, nvvidconv2, cap_filter, NULL);
  if (!gst_element_link_many (source, cap_filter1,nvvidconv1, nvvidconv2, cap_filter, NULL)) 
  {
    g_printerr ("Elements could not be linked: 1. Exiting.\n");
    return -1;
  }

  gst_bin_add (GST_BIN(pipeline), streammux);

  //********************* 将source_bin 添加到streammux元素的sink pad *********************
  GstPad *sinkpad, *srcpad;
  gchar pad_name_sink[16] = "sink_0";
  gchar pad_name_src[16] = "src";

  sinkpad = gst_element_get_request_pad (streammux, pad_name_sink);
  if (!sinkpad) 
  {
    g_printerr ("Streammux request sink pad failed. Exiting.\n");
    return -1;
  }

  srcpad = gst_element_get_static_pad (cap_filter, pad_name_src);
  if (!srcpad)
  {
    g_printerr ("Decoder request src pad failed. Exiting.\n");
    return -1;
  }

  if (gst_pad_link (srcpad, sinkpad) != GST_PAD_LINK_OK)
  {
    g_printerr ("Failed to link decoder to stream muxer. Exiting.\n");
    return -1;
  }

  gst_object_unref (sinkpad);
  gst_object_unref (srcpad);
  //********************* 将source_bin 添加到streammux元素的sink pad *********************

  gst_bin_add_many (GST_BIN (pipeline),pgie,nvtracker, nvvidconv, nvosd, transform, h264parser ,converter,encoder,muxer,sink, NULL);

  if (!gst_element_link(streammux,pgie))
  {
    g_printerr ("streammux,pgie could not be linked: Exiting.\n");
    return -1;
  }

  if (!gst_element_link(pgie,nvtracker))
  {
    g_printerr ("pgie,nvtracker could not be linked: Exiting.\n");
    return -1;
  }

  if (!gst_element_link(nvtracker,nvvidconv))
  {
    g_printerr ("nvtracker,nvvidconv could not be linked: Exiting.\n");
    return -1;
  }

  if (!gst_element_link(nvvidconv,nvosd))
  {
    g_printerr ("nvvidconv,nvosd could not be linked: Exiting.\n");
    return -1;
  }

  if (!gst_element_link(nvosd,converter))
  {
    g_printerr ("nvosd,converter could not be linked: Exiting.\n");
    return -1;
  }

  if (!gst_element_link(converter,encoder))
  {
    g_printerr ("converter encoder could not be linked: Exiting.\n");
    return -1;
  }

  if (!gst_element_link(encoder,h264parser))
  {
    g_printerr ("encoder,h264parser could not be linked: Exiting.\n");
    return -1;
  }

  if (!gst_element_link(h264parser,muxer))
  {
    g_printerr ("h264parser,muxer could not be linked: Exiting.\n");
    return -1;
  }

  if (!gst_element_link(muxer,sink))
  {
    g_printerr ("muxer,sink could not be linked: Exiting.\n");
    return -1;
  }

  // osd_sink_pad = gst_element_get_static_pad (nvosd, "sink");
  // if (!osd_sink_pad)
  // {g_print ("Unable to get sink pad\n");}
  // gst_pad_add_probe (osd_sink_pad, GST_PAD_PROBE_TYPE_BUFFER,osd_sink_pad_buffer_probe, NULL,NULL);
  // gst_object_unref (osd_sink_pad);

  // 添加探针，用于获取元数据
  osd_sink_pad = gst_element_get_static_pad(nvtracker, "src"); // 获取nvosd元素的sink pad
  if (!osd_sink_pad)
      g_print("Unable to get sink pad\n");
  else
  {
    // 参数：pad, 探针类型, 探针回调函数, 回调函数的参数, 回调函数的参数释放函数
    gst_pad_add_probe(osd_sink_pad, GST_PAD_PROBE_TYPE_BUFFER, osd_sink_pad_buffer_probe, NULL, NULL); // 添加探针
  // g_timeout_add(5000, perf_print_callback, &g_perf_data);      
  }
                                                // 添加定时器，用于打印性能数据
  gst_object_unref(osd_sink_pad);

  //add a message handler 
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  // Wait till pipeline encounters an error or EOS 
  g_print ("Running...\n");
  g_main_loop_run (loop);

  // Out of the main loop, clean up 
  g_print ("Returned, stopping playback\n");
  gst_element_set_state (pipeline, GST_STATE_NULL);
  g_print ("Deleting pipeline\n");

  gst_object_unref (GST_OBJECT (pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);
  return 0;
}