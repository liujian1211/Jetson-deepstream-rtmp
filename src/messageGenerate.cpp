#include "messageGenerate.h"

uint16_t messageGenerate::crc16(const std::vector<uint8_t>& bytes)
{
    uint16_t crc = 0xFFFF;
    for (size_t j = 0; j < bytes.size(); j++) 
    {
        crc = ((crc >> 8) | (crc << 8)) & 0xffff;
        crc ^= (bytes[j] & 0xff);
        crc ^= ((crc & 0xff) >> 4);
        crc ^= (crc << 12) & 0xffff;
        crc ^= ((crc & 0xFF) << 5) & 0xffff;
    }
    crc &= 0xffff;
    return crc;
}

std::string messageGenerateLocation::getCurrentTime()
{
    time_t current_time = time(nullptr);
    struct tm *localTime = localtime(&current_time);

    std::stringstream ss;
    ss<<localTime->tm_year +1900<<std::setw(2)<<std::setfill('0')<<localTime->tm_mon+1;
    ss<<std::setw(2)<<std::setfill('0')<<localTime->tm_mday;
    ss << std::setw(2) << std::setfill('0') << localTime->tm_hour;
    ss << std::setw(2) << std::setfill('0') << localTime->tm_min;
    ss << std::setw(2) << std::setfill('0') << localTime->tm_sec;
    std::string time_display = ss.str();
    return time_display;
}

int messageGenerateLocation::Str2Hex(std::string Str)
{    
    std::string numberStr;
    for (char c:Str)
    {
        if(std::isdigit(c) || c=='.' || c=='-')
        {
            numberStr += c; //得到去掉单位的结果，字符串形式
        }
    }

    //得到整数部分
    int integral_part = static_cast<int>(std::stof(numberStr));

    //得到小数部分
    double fractional_part = std::stof(numberStr) - integral_part;

    int fractional_hex = fractional_part * 0xFFFFFF; // 0xFFFFFF是16进制数的最大值

    // 合并整数部分和小数部分的十六进制数
    int total_hex = (integral_part << 24) | fractional_hex; // 将整数部分左移24位再与小数部分进行按位或操作
    
    // std::cout<<"16进制表示为"<<std::setw(8) << std::setfill('0')<<std::hex <<total_hex<<std::endl;

    return total_hex;
}