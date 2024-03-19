#include <iostream>
#include <string>
#include <bitset>
#include <vector>

#include <ctime>
#include <sstream>
#include <iomanip>


/**
 * 生成心跳报文的基类
*/
class messageGenerate
{
    private:
        
        int crcCheckCode;   //crc校验码（16进制）
        
        
    public:
        // messageGenerate();
        // ~messageGenerate();
        std::string header = "5B";
        std::string footer = "5D";
        std::string deviceNum = "018761869337";  //设备编号，以后初始化用，现在现写死
        
        uint16_t crc16(const std::vector<uint8_t>& bytes);
        // uint8_t* crcBytes(const uint8_t* bytes,size_t length);

};

/**
 * 心跳报文生成
 */
class messageGenerateHeartbeat:public messageGenerate
{
    private:    

    public:
        int length = 0x1C;
        int serialNum = 0x01;
        std::string heartbeatOrder = "1005";
        std::string versionNum = "010000";
        std::string encryption = "00" ; //是否加密
        std::string secretKey = "00000000"; //密钥
        
        // messageGenerateHeartbeat();
        // ~messageGenerateHeartbeat();
};

/**
 * 注册报文生成
*/
class messageGenerateRegister:public messageGenerate
{
    private:
        
    public:
        int length = 0x32;
        int serialNum = 0x01;
        std::string registerOrder = "1001";
        std::string versionNum = "010000";
        std::string encryption = "00"; //是否加密
        std::string secretKey = "00000000"; //密钥
        std::string carNum = "CBD544363031444400000000000000000000000000"; //16进制的车牌号
        std::string carColor = "01";

        // messageGenerateRegister();
        // ~messageGenerateRegister();
};

/**
 * 实时定位报文生成
*/
class messageGenerateLocation:public messageGenerate
{
    private:
        

    public:
        int length = 0x31;
        int serialNum = 0x01;
        std::string locationOrder = "1003";
        std::string versionNum = "010000";
        std::string encryption ; //是否加密
        std::string secretKey = "00000000"; //密钥
        std::string date; //日期，年月日时分秒
        int longitude = 0x071B5B29; //经度，16进制，转换为十进制是119233321
        int latitude = 0x020DBB3C;  //纬度，16进制
        int velocity = 0x000C;  //速度，16进制
        int direction = 0x000F; //方向
        int altitude = 0x0020;  //海拔

        // messageGenerateLocation();
        // ~messageGenerateLocation();
        std::string getCurrentTime();
        int Str2Hex(std::string Str);
      
};

/**
 * 视频打开回复 报文生成
*/
class messageGenerateVideoOpen:public messageGenerate
{
    private:
        
    public:
        int length = 0x1D;
        int serialNum = 0x01;
        std::string videoOpenOrder = "9004";
        std::string versionNum = "010000";
        std::string encryption = "00" ; //是否加密
        std::string secretKey = "00000000"; //密钥
        std::string success = "00";

        // messageGenerateVideoOpen();
        // ~messageGenerateVideoOpen();
};

/**
 * 视频关闭回复 报文生成
*/
class messageGenerateVideoClose:public messageGenerate
{
    private:
        
    public:
        int length = 0x1D;
        int serialNum = 0x01;
        std::string videoCloseOrder = "9006";
        std::string versionNum = "010000"; //版本号
        std::string encryption = "00"; //是否加密
        std::string secretKey = "00000000"; //密钥
        std::string success = "00";

        // messageGenerateVideoClose();
        // ~messageGenerateVideoClose();
};

/**
 * 实时视频心跳回复 报文生成
*/
class messageGenerateRealtimeVideo:public messageGenerate
{
    private:
        
    public:
        int length = 0x1C;
        int serialNum = 0x01;
        std::string realtimeVideoOrder = "9002";
        std::string versionNum = "010000"; //版本号
        std::string encryption = "00"; //是否加密
        std::string secretKey = "00000000"; //密钥

        // messageGenerateRealtimeVideo();
        // ~messageGenerateRealtimeVideo();
};