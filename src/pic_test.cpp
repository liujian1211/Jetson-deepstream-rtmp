#include <ctime>
#include "pic_test.h"
#include "iostream"
#include <sys/socket.h>
// #include <thread>
#include <chrono>
#include <arpa/inet.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
#include "libavutil/frame.h"
#include <libavutil/imgutils.h>
}

#include <libusb-1.0/libusb.h>

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <glib.h>

#define WIDTH 1920
#define HEIGHT 1080
#define FPS 30
#define BITRATE 2000000

#define RTMP_SERVER "rtmp://112.82.244.90:1935/live/vehicle_1"

using namespace std;

//********************用于rtmp传输********************
// const char* video_path = "/opt/nvidia/deepstream/deepstream-5.0/samples/streams/dexing_0925.flv";
const char* video_path = "/dev/video0";
const char* url = "rtmp://112.82.244.90:1935/live/vehicle_1";
//********************用于rtmp传输********************

//在图片左上角显示当前时间
cv::Mat putDate(cv::Mat frame) {
    cv::rectangle(frame, cv::Rect(0, 0, 320, 22), cv::Scalar(0, 0, 0), -1);
    std::time_t now = std::time(nullptr);
    std::tm* current_time = std::localtime(&now);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", current_time);
    cv::putText(frame, buf, cv::Point(0, 20), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.3,
                cv::Scalar(255, 255, 255), 2, cv::LINE_4);
    return frame;
}

void socket_func()
{
    int client = socket(AF_INET,SOCK_DGRAM,0);
    try
    {
        struct sockaddr_in serverAddr{};    
        std::string host = "10.0.1.34";
        int port = 8888;

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &(serverAddr.sin_addr));

        cout<<"正在连接。。。"<<endl;
        if (connect(client, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            throw std::runtime_error("连接失败");
        }
        std::cout << "连接成功，工作中~" << std::endl;

        while (true) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            cv::Mat img = cv::imread("/home/nano/liujian/test_websocket/yolov3.jpg");
            if (img.empty()) {
                throw std::runtime_error("无法读取图片");
            }

            putDate(img);
            // cv::imshow("JNano", img);
            // if (cv::waitKey(1) & 0xFF == 'q') {
            //     break;
            // }

            std::vector<uchar> send_data;
            cv::imencode(".jpg", img, send_data, {cv::IMWRITE_JPEG_QUALITY, 50});
            sendto(client, send_data.data(), send_data.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        }

    }

    catch(const std::exception& e)
    {
        std::cerr << "发送失败: " << e.what() << std::endl;
        std::cout << "即将重试" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        socket_func();
    }
    close(client);
}

int rtmp_func()
{
    // 初始化输入和输出格式上下文
    AVFormatContext* inputFormatContext = nullptr;
    AVFormatContext* outputFormatContext = nullptr;

    if (avformat_open_input(&inputFormatContext, video_path, nullptr, nullptr) < 0)
    {
        std::cerr << "Failed to open input file" << std::endl;
        return -1;
    }

    if (avformat_find_stream_info(inputFormatContext, nullptr) < 0)
    {
        std::cerr << "Failed to retrieve input stream information" << std::endl;
        avformat_close_input(&inputFormatContext);
        return -1;
    }

    if (avformat_alloc_output_context2(&outputFormatContext, nullptr, "flv", url) < 0)
    {
        std::cerr << "Failed to create output context" << std::endl;
        avformat_close_input(&inputFormatContext);
        return -1;
    }

    // 遍历输入流并复制到输出流
    for (unsigned int i = 0; i < inputFormatContext->nb_streams; i++)
    {
        AVStream* inputStream = inputFormatContext->streams[i];
        AVStream* outputStream = avformat_new_stream(outputFormatContext, nullptr);
        if (!outputStream)
        {
            std::cerr << "Failed to allocate output stream" << std::endl;
            avformat_close_input(&inputFormatContext);
            avformat_free_context(outputFormatContext);
            return -1;
        }

        if (avcodec_parameters_copy(outputStream->codecpar, inputStream->codecpar) < 0)
        {
            std::cerr << "Failed to copy codec parameters" << std::endl;
            avformat_close_input(&inputFormatContext);
            avformat_free_context(outputFormatContext);
            return -1;
        }
    }

    // 打开输出URL
    if (!(outputFormatContext->oformat->flags & AVFMT_NOFILE))
    {
        if (avio_open2(&outputFormatContext->pb, url, AVIO_FLAG_WRITE, nullptr, nullptr) < 0)
        {
            std::cerr << "Failed to open output URL" << std::endl;
            avformat_close_input(&inputFormatContext);
            avformat_free_context(outputFormatContext);
            return -1;
        }
    }

    // 写入输出文件头
    if (avformat_write_header(outputFormatContext, nullptr) < 0)
    {
        std::cerr << "Failed to write output file header" << std::endl;
        avformat_close_input(&inputFormatContext);
        avformat_free_context(outputFormatContext);
        return -1;
    }

    // 读取数据包并写入输出文件
    AVPacket packet;
    while (av_read_frame(inputFormatContext, &packet) >= 0)
    {
        if (packet.stream_index >= 0 && packet.stream_index < outputFormatContext->nb_streams)
        {
            AVStream* outputStream = outputFormatContext->streams[packet.stream_index];

            // 计算时间基
            AVRational timeBaseQ = inputFormatContext->streams[packet.stream_index]->time_base;
            packet.pts = av_rescale_q_rnd(packet.pts, timeBaseQ, outputStream->time_base, AVRounding(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            packet.dts = av_rescale_q_rnd(packet.dts, timeBaseQ, outputStream->time_base, AVRounding(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            packet.duration = av_rescale_q(packet.duration, timeBaseQ, outputStream->time_base);
            packet.pos = -1;

            // 写入数据包
            if (av_interleaved_write_frame(outputFormatContext, &packet) < 0)
            {
                std::cerr << "Failed to write packet" << std::endl;
                break;
            }
          
            av_packet_unref(&packet);

        }
        av_packet_unref(&packet);
    }

    // 写入输出文件尾部
    av_write_trailer(outputFormatContext);
    // 清理资源
    avformat_close_input(&inputFormatContext);
    if (outputFormatContext && !(outputFormatContext->oformat->flags & AVFMT_NOFILE))
    {
        avio_closep(&outputFormatContext->pb);
    }
    avformat_free_context(outputFormatContext);

    return 0;
}



int camera_open()
{

    // 打开USB相机
    cv::VideoCapture cap(0);
    
    // 检查相机是否成功打开
    if (!cap.isOpened()) {
        std::cout << "无法打开相机" << std::endl;
        return -1;
    }

    cv::Mat frame;
    
    while (true) {
        // 从相机中读取一帧
        cap.read(frame);
        
        // 检查是否成功读取帧
        if (frame.empty()) {
            std::cout << "无法从相机中读取帧" << std::endl;
            break;
        }
        
        // 在窗口中显示帧
        cv::imshow("USB相机", frame);
        
        // 按下ESC键退出循环
        if (cv::waitKey(1) == 27) {
            break;
        }
    }
    
    // 释放相机对象
    cap.release();

    // 关闭窗口
    cv::destroyAllWindows();
    
    return 0;

}

// 回调函数，用于接收推理结果
static GstFlowReturn newSampleCallback(GstAppSink *appsink, gpointer user_data) {
    GstSample *sample = gst_app_sink_pull_sample(appsink);
    GstCaps *caps = gst_sample_get_caps(sample);
    GstStructure *structure = gst_caps_get_structure(caps, 0);

    // 获取推理结果中的metadata和图像数据
    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo mapinfo;
    gst_buffer_map(buffer, &mapinfo, GST_MAP_READ);

    // 在这里可以对推理结果进行处理


    gst_buffer_unmap(buffer, &mapinfo);
    gst_sample_unref(sample);

    return GST_FLOW_OK;
}

int deepstream_trans()
{
    // gst_init(&argc, &argv);

    // 创建GStreamer管道
    GstElement *pipeline = gst_pipeline_new("pipeline");

    // 创建元素
    GstElement *source = gst_element_factory_make("v4l2src", "source");
    GstElement *capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
    GstElement *decoder = gst_element_factory_make("decodebin", "decoder");
    GstElement *appsink = gst_element_factory_make("appsink", "appsink");

    if (!pipeline || !source || !capsfilter || !decoder || !appsink) {
        std::cerr << "Failed to create GStreamer elements" << std::endl;
        return -1;
    }

    // 设置v4l2src参数
    g_object_set(G_OBJECT(source), "device", "/dev/video0", NULL);

    // 设置capsfilter参数，指定图像格式
    GstCaps *caps = gst_caps_from_string("video/x-raw, format=(string)NV12");
    g_object_set(G_OBJECT(capsfilter), "caps", caps, NULL);

    // 设置decodebin的回调函数，用于在解码器准备好后链接到appsink
    g_signal_connect(decoder, "pad-added", G_CALLBACK([](GstElement *element, GstPad *pad, gpointer user_data) {
        GstPad *sinkpad = gst_element_get_static_pad(static_cast<GstElement*>(user_data), "sink");
        gst_pad_link(pad, sinkpad);
        gst_object_unref(sinkpad);
    }), appsink);

    // 设置appsink的参数，用于接收推理结果
    g_object_set(G_OBJECT(appsink), "emit-signals", TRUE, NULL);
    g_signal_connect(appsink, "new-sample", G_CALLBACK(newSampleCallback), NULL);

    // 添加元素到管道
    gst_bin_add_many(GST_BIN(pipeline), source, capsfilter, decoder, appsink, NULL);

    // 链接元素
    if (!gst_element_link_many(source, capsfilter, decoder, NULL)) {
        std::cerr << "Failed to link GStreamer elements" << std::endl;
        gst_object_unref(pipeline);
        return -1;
    }

    // 启动管道
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // 主事件循环
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    // 停止管道
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}