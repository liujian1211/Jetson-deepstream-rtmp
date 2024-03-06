#include "tcp_ip_message.h"
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sstream>
#include <iomanip>
#include "messageGenerate.h"
#include "getGPS.h"

// #define SERVER_IP "10.0.1.34"   //定义服务器IP
// #define SERVER_PORT 8080     //定义服务器端口

#define SERVER_IP "112.82.244.92"   //定义服务器IP
#define SERVER_PORT 7872     //定义服务器端口

#define BUFFER_SIZE 1024     //设置接收报文类型函数的报文的长度
#define BUFFER_SIZE_HEARTBEAT 1024 //设置接收心跳报文的长度

messageGenerate::messageGenerate(){}
messageGenerate::~messageGenerate(){}
messageGenerate MG; //实例化 生成报文类

messageGenerateHeartbeat::messageGenerateHeartbeat(){}
messageGenerateHeartbeat::~messageGenerateHeartbeat(){}
messageGenerateHeartbeat MGheartbeat; //实例化 生成心跳报文类

messageGenerateLocation::messageGenerateLocation(){}
messageGenerateLocation::~messageGenerateLocation(){}
messageGenerateLocation MGlocation; //实例化 生成定位报文类

messageGenerateRegister::messageGenerateRegister(){}
messageGenerateRegister::~messageGenerateRegister(){}
messageGenerateRegister MGregister; //实例化 生成注册报文类

messageGenerateVideoOpen::messageGenerateVideoOpen(){}
messageGenerateVideoOpen::~messageGenerateVideoOpen(){}
messageGenerateVideoOpen MGvideoopen; //实例化 生成视频打开报文类

messageGenerateVideoClose::messageGenerateVideoClose(){}
messageGenerateVideoClose::~messageGenerateVideoClose(){}
messageGenerateVideoClose MGvideoclose; //实例化 生成视频关闭报文类

messageGenerateRealtimeVideo::messageGenerateRealtimeVideo(){}
messageGenerateRealtimeVideo::~messageGenerateRealtimeVideo(){}
messageGenerateRealtimeVideo MGrealtimevideo; //实例化 生成实时视频心跳报文类

// getGPS::getGPS(){}
// getGPS::~getGPS(){}
getGPS GG;  //实例化 获取当前GPS

/**
 * 执行cmd命令行，使用分离线程
*/
void executeCommand()
{   
    system("sh /home/nano/startup.sh");
}

/**
 * 终止上面的cmd命令，也使用分离线程
*/
// void terminateCommand()
// {
//     // 终止指定进程
//     system("pkill -f /home/nano/startup.sh"); 
// }

/**
 * 判断接收的报文类型：心跳、定位、视频打开、视频关闭、实时视频
*/
int MessageType(int sockfd)
{
    char buffer[BUFFER_SIZE];
    memset(buffer,0,BUFFER_SIZE);
    int bytesRead;

    while(true)
    {
        // **************************************组合 视频打开报文**************************************
        //将int类型的length转为16进制
        std::stringstream ssVideoOpenLength;
        ssVideoOpenLength << std::setw(8) << std::setfill('0') << std::hex << MGvideoopen.length;

        //将int类型的serialNum转为16进制
        std::stringstream ssVideoOpenSerialNum;
        ssVideoOpenSerialNum << std::setw(8) << std::setfill('0') << std::hex << MGvideoopen.serialNum;

        // 将上述内容组合为一个body
        std::string videoopenMsgBody = ssVideoOpenLength.str() + ssVideoOpenSerialNum.str() + 
                                MGvideoopen.videoOpenOrder + MGvideoopen.deviceNum + 
                                MGvideoopen.versionNum + MGvideoopen.encryption + MGvideoopen.secretKey+
                                MGvideoopen.success;
        const char* videoopenMsgBody_ = videoopenMsgBody.c_str();

        //将上述body生成CRC校验码
        std::vector<uint8_t>bytesVideoOpen;
        for (size_t i = 0; i < strlen(videoopenMsgBody_); i += 2)
        {
            uint8_t byte = std::stoi(std::string(videoopenMsgBody_).substr(i, 2), nullptr, 16);
            bytesVideoOpen.push_back(byte);
        }
        uint16_t crcValueVideoOpen = MG.crc16(bytesVideoOpen);
        std::stringstream ssVideoOpen;
        ssVideoOpen << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << crcValueVideoOpen;
        // std::cout<<"心跳生成的校验码为"<<ssVideoOpen.str()<<std::endl;
    
        //上述报文的进制转换
        std::string videoopenMsg_ = MG.header + videoopenMsgBody + ssVideoOpen.str() +MG.footer;
        const char* videoopenMsg = videoopenMsg_.c_str();
        
        // **************************************组合 视频打开报文**************************************


        // **************************************组合 视频关闭报文**************************************
        //将int类型的length转为16进制
        std::stringstream ssVideoCloseLength;
        ssVideoCloseLength << std::setw(8) << std::setfill('0') << std::hex << MGvideoclose.length;

        //将int类型的serialNum转为16进制
        std::stringstream ssVideoCloseSerialNum;
        ssVideoCloseSerialNum << std::setw(8) << std::setfill('0') << std::hex << MGvideoclose.serialNum;

        // 将上述内容组合为一个body
        std::string videocloseMsgBody = ssVideoCloseLength.str() + ssVideoCloseSerialNum.str() + 
                                MGvideoclose.videoCloseOrder + MGvideoclose.deviceNum + 
                                MGvideoclose.versionNum + MGvideoclose.encryption + MGvideoclose.secretKey+
                                MGvideoclose.success;
        const char* videocloseMsgBody_ = videocloseMsgBody.c_str();

        //将上述body生成CRC校验码
        std::vector<uint8_t>bytesVideoClose;
        for (size_t i = 0; i < strlen(videocloseMsgBody_); i += 2)
        {
            uint8_t byte = std::stoi(std::string(videocloseMsgBody_).substr(i, 2), nullptr, 16);
            bytesVideoClose.push_back(byte);
        }
        uint16_t crcValueVideoClose = MG.crc16(bytesVideoClose);
        std::stringstream ssVideoClose;
        ssVideoClose << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << crcValueVideoClose;
        // std::cout<<"心跳生成的校验码为"<<ssVideoClose.str()<<std::endl;

        //上述报文的进制转换
        std::string videocloseMsg_ = MG.header + videocloseMsgBody + ssVideoClose.str() + MG.footer;
        const char* videocloseMsg = videocloseMsg_.c_str();
        
        // **************************************组合 视频关闭报文**************************************
        
        
        // **************************************组合 实时视频心跳报文**************************************
        //将int类型的length转为16进制
        std::stringstream ssRealtimeVideoLength;
        ssRealtimeVideoLength << std::setw(8) << std::setfill('0') << std::hex << MGrealtimevideo.length;

        //将int类型的serialNum转为16进制
        std::stringstream ssRealtimeVideoSerialNum;
        ssRealtimeVideoSerialNum << std::setw(8) << std::setfill('0') << std::hex << MGrealtimevideo.serialNum;

        // 将上述内容组合为一个body
        std::string realtimeVideoMsgBody = ssRealtimeVideoLength.str() + ssRealtimeVideoSerialNum.str() + 
                                MGrealtimevideo.realtimeVideoOrder + MGrealtimevideo.deviceNum + 
                                MGrealtimevideo.versionNum + MGrealtimevideo.encryption + MGrealtimevideo.secretKey;
        const char* realtimeVideoMsgBody_ = realtimeVideoMsgBody.c_str();

        //将上述body生成CRC校验码
        std::vector<uint8_t>bytesRealtimeVideo;
        for (size_t i = 0; i < strlen(realtimeVideoMsgBody_); i += 2)
        {
            uint8_t byte = std::stoi(std::string(realtimeVideoMsgBody_).substr(i, 2), nullptr, 16);
            bytesRealtimeVideo.push_back(byte);
        }
        uint16_t crcValueRealtimeVideo = MG.crc16(bytesRealtimeVideo);
        std::stringstream ssRealtimeVideo;
        ssRealtimeVideo << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << crcValueRealtimeVideo;
        // std::cout<<"心跳生成的校验码为"<<ssRealtimeVideo.str()<<std::endl;

        //上述报文的进制转换
        std::string realtimeVideoMsg_ = MG.header + realtimeVideoMsgBody + ssRealtimeVideo.str() + MG.footer;
        const char* realtimeVideoMsg = realtimeVideoMsg_.c_str();
        // **************************************组合 实时视频心跳报文**************************************
        
        // 上述组合好后的报文的进制转换
        std::string message_videoopen_hex;
        std::string message_videoclose_hex;
        std::string message_realtimevideo_hex;

        for (int i=0;i<strlen(videoopenMsg);i+=2)
        {
            int hexValue;
            sscanf(videoopenMsg+i,"%2X",&hexValue);
            message_videoopen_hex+=static_cast<char>(hexValue);
        }

        for (int i=0;i<strlen(videocloseMsg);i+=2)
        {
            int hexValue;
            sscanf(videocloseMsg+i,"%2X",&hexValue);
            message_videoclose_hex+=static_cast<char>(hexValue);
        }

        for (int i=0;i<strlen(realtimeVideoMsg);i+=2)
        {
            int hexValue;
            sscanf(realtimeVideoMsg+i,"%2X",&hexValue);
            message_realtimevideo_hex+=static_cast<char>(hexValue);
        }

        bytesRead = recv(sockfd,buffer,sizeof(buffer),0);
        //接收到的报文为16进制，需要做进制转换，即16进制->字符串

        std::stringstream ss;
        for(int i=0;i<bytesRead;i++)
        {
            ss<<std::hex<<std::setw(2)<<std::setfill('0')<<static_cast<int>(static_cast<unsigned char>(buffer[i]));
        }

        if (bytesRead>0)
        {
            std::cout<<"接收到的报文为"<<ss.str()<<std::endl;

            std::thread commandThread;
            

            char content[5] ;  //取出报文的第19~22位，用以判断报文的类型
            for (int i=0;i<4;i++)
            {
                content[i] = ss.str()[18+i];
            }

            int cont = atoi(content);

            // std::cout<<"cont内容为"<<cont<<std::endl;

            switch(cont)
            {
                case 1006: //心跳回复 服务-设备
                    std::cout<<"接收到总心跳回复"<<std::endl;
                    break;            
                
                case 1004: //定位回复 服务-设备
                    std::cout<<"接收到定位报文回复"<<std::endl;
                    break;

                case 9003: //视频打开请求 服务-设备
                    std::cout<<"接收到视频打开操作请求心跳"<<std::endl;
                    //问题，这步不应该是启动target，而是启动target运行后的视频传输，target应该是一直在执行的
                    if (send(sockfd,message_videoopen_hex.c_str(),message_videoopen_hex.length(),0) == -1)
                    {
                        std::cerr <<"发送视频打开回复报文失败"<<std::endl;
                        break;
                    }
                    else
                    {
                        std::cout<<"发送视频打开回复报文成功"<<std::endl;
                        std::thread t(executeCommand);
                        t.detach(); //分离线程，使线程在后台运行，不会阻塞主线程
                        
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        MGvideoopen.serialNum++;
                        break;
                    }
                      
                case 9005: //视频关闭请求 服务-设备
                    std::cout<<"接收到视频关闭操作请求心跳"<<std::endl;
                    //问题：该操作不应该重新执行system，而是应该关闭视频传输
                    if (send(sockfd,message_videoclose_hex.c_str(),message_videoclose_hex.length(),0)==-1)
                    {
                        std::cerr <<"发送视频关闭回复报文失败"<<std::endl;
                        break;
                    }
                    else
                    {                      
                        std::cout<<"发送视频关闭回复报文成功"<<std::endl;
                        // std::thread t(terminateCommand);
                        // t.detach();
                        
                        MGvideoclose.serialNum++;
                        break;
                    }
                        
                    

                case 9001: //实时视频请求 服务-设备
                    std::cout<<"接收到实时视频回复心跳"<<std::endl;
                    
                    // while(true)
                    // {
                        // if((time(NULL)%120)==0) //每隔120s发送
                        // {
                            if (send(sockfd, message_realtimevideo_hex.c_str(), message_realtimevideo_hex.length(), 0) == -1)
                            {
                                std::cerr << "发送实时视频回复报文失败" << std::endl;
                                break;
                            } 
                            else
                            {
                                std::cout<<"发送实时视频回复报文成功"<<std::endl;
                                MGrealtimevideo.serialNum++;
                                break;
                            }
                                
                        // }
                        
                    // }                 
            }
  
        }
        else
        {
            std::cerr<<"接收报文失败"<<std::endl;
            return -1;
        } 
    }   
}
 
/**
 * 发送心跳报文
*/
void sendHeartbeat(int sockfd) 
{  
    //将int类型的length转为16进制
    std::stringstream ssLength;
    ssLength << std::setw(8) << std::setfill('0') << std::hex << MGheartbeat.length;
    
    while (true) 
    {
        //将int类型的serialNum转为16进制
        std::stringstream ssSerialNum;
        ssSerialNum <<std::setw(8) << std::setfill('0')<<std::hex <<MGheartbeat.serialNum;

        //将上述内容组合为一个body，放入CRC校验码算法中，用于计算最终的CRC校验码
        std::string heartbeatMsgBody = ssLength.str() + ssSerialNum.str() + MGheartbeat.heartbeatOrder + 
                                    MGheartbeat.deviceNum + MGheartbeat.versionNum + MGheartbeat.encryption +
                                    MGheartbeat.secretKey;

        // 根据以上body生成CRC校验码
        const char* heartbeatMsgBody_ = heartbeatMsgBody.c_str();
        std::vector<uint8_t>bytes;
        for (size_t i = 0; i < strlen(heartbeatMsgBody_); i += 2)
        {
            uint8_t byte = std::stoi(std::string(heartbeatMsgBody_).substr(i, 2), nullptr, 16);
            bytes.push_back(byte);
        }
        uint16_t crcValue = MG.crc16(bytes);
        std::stringstream ss;
        ss << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << crcValue;
        // std::cout<<"心跳生成的校验码为"<<ss.str()<<std::endl;
        
        //心跳报文的进制转换
        std::string heartbeatMsg_ = MG.header + heartbeatMsgBody + ss.str() +MG.footer;
        const char* heartbeatMsg = heartbeatMsg_.c_str();
        std::string message_heartbeat_hex;
        for(int i=0;i<strlen(heartbeatMsg);i+=2)
        {
            int hexValue;
            sscanf(heartbeatMsg+i, "%2X",&hexValue);
            message_heartbeat_hex+=static_cast<char>(hexValue);
        }

         // 每隔10秒发送心跳报文
        if((time(NULL)%10)==0)
        {
            if (send(sockfd, message_heartbeat_hex.c_str(), message_heartbeat_hex.length(), 0) == -1)
            {
                std::cerr << "发送总心跳报文失败" << std::endl;
                break;
            }
            else
            {
                std::cout<<"发送总心跳报文成功"<< std::endl; 
                MGheartbeat.serialNum++; //每次发完心跳都要递增
            }               
        }
        // 等待一段时间，应该是1秒
        std::this_thread::sleep_for(std::chrono::seconds(1));               
    }
}

/**
 * 发送定位报文
*/
void sendLocation(int sockfd) 
{
    
    //将int类型的length转为16进制，length是固定不变的
    std::stringstream ssLength;
    ssLength << std::setw(8) << std::setfill('0') << std::hex << MGlocation.length;  
    ssLength.str();

    // 打开GPS，打开一次就可以
    int serial_port = GG.gpsOpen();
    
    time_t lastOutputTime = time(NULL); // 记录上一次输出的时间戳
    GG.getGPSData(serial_port,[&sockfd, &ssLength,&lastOutputTime](std::vector<std::string>result)
    {
        if(std::stof(result[1]) && std::stof(result[2]))    //当获取到经纬度的时候
        {          
            time_t currentTime = time(NULL);  //获取当前时间戳
            // if((time(NULL)%20)==0)  //每隔20s更新一次
            if(currentTime - lastOutputTime >=20)
            {
                lastOutputTime = currentTime; //更新上次输出的时间戳
                // std::cout<<"经度为"<< result[1]<<",16进制表示为"<<std::setw(8) << std::setfill('0') << std::hex <<MGlocation.Str2Hex(result[1])<<std::endl;
                // std::cout<<"纬度为"<< result[2]<<",16进制表示为"<<std::setw(8) << std::setfill('0') << std::hex <<MGlocation.Str2Hex(result[2])<<std::endl;
                // std::cout<<"海拔为"<< result[4]<<",16进制表示为"<<std::setw(8) << std::setfill('0') << std::hex <<MGlocation.Str2Hex(result[4])<<std::endl;
                // std::cout<<"真北朝向为"<<result[5]<<",16进制表示为"<<std::setw(8) << std::setfill('0') << std::hex <<MGlocation.Str2Hex(result[5])<<std::endl;  
                // std::cout<<"速度为"<<result[8]<<",16进制表示为"<<std::setw(8) << std::setfill('0') << std::hex <<MGlocation.Str2Hex(result[2])<<std::endl;
                // std::cout<<std::endl;
            
                //将int类型的serialNum转为16进制
                std::stringstream ssSerialNum;
                ssSerialNum <<std::setw(8) << std::setfill('0')<<std::hex <<MGlocation.serialNum;
                ssSerialNum.str();

                // 将int类型的经度转为16进制
                std::stringstream ssLongitude;
                // ssLongitude << std::setw(8) << std::setfill('0') << std::hex << MGlocation.longitude;
                ssLongitude << std::setw(8) << std::setfill('0') << std::hex << MGlocation.Str2Hex(result[1]);

                // 将int类型的纬度转为16进制
                std::stringstream ssLatitude;
                // ssLatitude << std::setw(8) << std::setfill('0') << std::hex << MGlocation.latitude;
                ssLatitude << std::setw(8) << std::setfill('0') << std::hex << MGlocation.Str2Hex(result[2]);

                // 将int类型的速度转为16进制
                std::stringstream ssVelocity;
                // ssVelocity << std::setw(8) << std::setfill('0') << std::hex << MGlocation.velocity;
                ssVelocity << std::setw(8) << std::setfill('0') << std::hex << MGlocation.Str2Hex(result[8]);

                // 将int类型的方向转为16进制
                std::stringstream ssDirection;
                // ssDirection << std::setw(8) << std::setfill('0') << std::hex << MGlocation.direction;
                ssDirection << std::setw(8) << std::setfill('0') << std::hex << MGlocation.Str2Hex(result[5]);

                // 将int类型的海拔转为16进制
                std::stringstream ssAltitude;
                // ssAltitude << std::setw(8) << std::setfill('0') << std::hex << MGlocation.altitude;
                ssAltitude << std::setw(8) << std::setfill('0') << std::hex << MGlocation.Str2Hex(result[4]);

                // 给日期赋值
                MGlocation.date = MGlocation.getCurrentTime();  

                //将上述以及其它内容组合为一个body，放入CRC校验码算法中，用于计算最终的CRC校验码
                std::string locationMsgBody = ssLength.str() + ssSerialNum.str() + MGlocation.locationOrder + 
                                            MGlocation.deviceNum + MGlocation.versionNum + MGlocation.encryption +
                                            MGlocation.secretKey + MGlocation.date + ssLongitude.str() +
                                            ssLatitude.str() + ssVelocity.str() + ssDirection.str() +
                                            ssAltitude.str();
                const char* locationMsgBody_ = locationMsgBody.c_str();

                //根据以上body生成CRC校验码
                std::vector<uint8_t>bytes;
                for (size_t i = 0; i < strlen(locationMsgBody_); i += 2)
                {
                    uint8_t byte = std::stoi(std::string(locationMsgBody_).substr(i, 2), nullptr, 16);
                    bytes.push_back(byte);
                }
                uint16_t crcValue = MG.crc16(bytes);
                std::stringstream ss;
                ss << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << crcValue;
                // std::cout<<"定位心跳生成的校验码为"<<ss.str()<<std::endl;
                    
                //定位报文的进制转换
                std::string locationMsg_ = MGlocation.header + locationMsgBody + ss.str() + MG.footer;
                const char* locationMsg = locationMsg_.c_str();
                std::string message_location_hex;
                for(int i=0;i<strlen(locationMsg);i+=2)
                {
                    int hexValue;
                    sscanf(locationMsg+i, "%2X",&hexValue);
                    message_location_hex+=static_cast<char>(hexValue);
                }
                
                //处理完报文后就发送
                if (send(sockfd, message_location_hex.c_str(), message_location_hex.length(), 0) == -1)
                {
                    std::cerr << "发送定位报文失败" << std::endl;
                }
                else
                {
                    std::cout<<"发送定位报文成功"<< std::endl; 
                    MGlocation.serialNum++;
                }              
            }
        }
    });

}

int message()
{
    // 创建socket
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd == -1)
    {
        std::cerr << "创建socket失败!" << std::endl;
        return -1;
    }

    // 设置服务器信息
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(serverAddr.sin_addr)) <= 0)
    {
        std::cerr << "无效或不支持的IP" << std::endl;
        close(sockfd);
        return -1;
    }

    // 连接服务器
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) 
    {
        std::cerr << "连接服务器失败" << std::endl;
        close(sockfd);
        return -1;
    }   

    // 组合注册报文,然后发送
    //将int类型的length转为16进制
    std::stringstream ssLength;
    ssLength << std::setw(8) << std::setfill('0') << std::hex << MGregister.length;

    //将int类型的serialNum转为16进制
    std::stringstream ssSerialNum;
    ssSerialNum <<std::setw(8) << std::setfill('0')<<std::hex <<MGregister.serialNum;

    //将上述及注册报文的其它内容组合为一个body，放入CRC校验码算法中，用于计算最终的CRC校验码 
    std::string registerMsgBody = ssLength.str() + ssSerialNum.str() + MGregister.registerOrder +
                                  MGregister.deviceNum + MGregister.versionNum + MGregister.encryption + 
                                  MGregister.secretKey + MGregister.carNum + MGregister.carColor; 
    const char* registerMsgBody_ = registerMsgBody.c_str();

    // 将上述组合而成的body放入CRC校验码算法中
    std::vector<uint8_t>bytes;
    for (size_t i = 0; i < strlen(registerMsgBody_); i += 2)
    {
        uint8_t byte = std::stoi(std::string(registerMsgBody_).substr(i, 2), nullptr, 16);
        bytes.push_back(byte);
    }
    uint16_t crcValue = MG.crc16(bytes);
    std::stringstream ssRegister;
    ssRegister << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << crcValue;

    //注册报文的进制转换
    std::string registerMsg_ = MG.header + registerMsgBody + ssRegister.str() + MG.footer;
    const char* registerMsg = registerMsg_.c_str();
    std::string message_register_hex;
    for(int i=0;i<strlen(registerMsg);i+=2)
    {
        int hexValue;
        sscanf(registerMsg+i,"%2X",&hexValue);
        message_register_hex+=static_cast<char>(hexValue);
    }
    
    if (send(sockfd, message_register_hex.c_str(), message_register_hex.length(), 0) == -1)
    {
        std::cerr << "发送注册报文失败" << std::endl;
        //TO DO:等待3s后再次发送注册报文
        close(sockfd);
        return -1;
    }
    else
    {
        std::cout<<"发送注册报文成功, 等待注册报文回复..."<<std::endl;

        MGregister.serialNum++;

        char buffer_register_reply[BUFFER_SIZE];   
        memset(buffer_register_reply,0,BUFFER_SIZE);
        int bytesRead_register_reply;
        bytesRead_register_reply = recv(sockfd,buffer_register_reply,sizeof(buffer_register_reply),0); 
        //因为是16进制,需做进制转换，即16进制->字符串
                
        std::stringstream ss;
        for (int i=0;i<bytesRead_register_reply;i++)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(buffer_register_reply[i]));
        }

        // std::cout<<"接收到的字符串为"<<ss.str()<<std::endl;
        
        if (bytesRead_register_reply>0)
        {           
            char content[5];
            for (int i=0;i<4;i++)
            {
                content[i] = ss.str()[18+i];
            }
            int cont = atoi(content);

            std::cout<<"收到注册报文的回复指令："<<cont<<std::endl;

            if(cont == 1002)  //注册回复指令，后期应该为变量
            {
                // 起心跳发送线程
                std::thread sendheartbeatThread(sendHeartbeat,sockfd);

                // 起定位报文发送线程
                std::thread sendLocationThread(sendLocation,sockfd);

                // 起接收报文线程
                std::thread recvMsgThread(MessageType,sockfd);

                // 等待心跳发送线程结束
                sendheartbeatThread.join();

                // 等待定位报文发送线程结束
                sendLocationThread.join();

                //等待接收报文线程结束
                recvMsgThread.join();
            }
            else
                std::cout<<"注册回复不为1002"<<std::endl;
        }
        else
            std::cout<<"未收到注册报文回复"<<std::endl;

    }

// ****************************************************************************************************************************
    //若心跳回复报文过长，则time out，需重新注册
    
    // 关闭socket
    close(sockfd);

}

int main()
{
    message();
}