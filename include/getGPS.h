#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <cstring>
#include <thread>
#include <SerialStream.h>
#include <vector>
#include <functional>
#include <mutex>
using namespace LibSerial;

class getGPS
{
    private:
        struct termios tty;
        char data;

        char buffer_[256];

    public:
      
        // getGPS();
        // ~getGPS();
        int gpsOpen();

        // 获取GPS数据的回调函数
        using Callback = std::function<void(const std::vector<std::string>&)>;
        // std::vector <std::string> getGPSData(int serial_fd, const Callback& callback);     
        void getGPSData(int serial_fd,const Callback& callback);  

        double convert2Degrees(std::string in_data1,std::string in_data2);
        std::vector <std::string> parseLine(std::string input);

        static std::vector<std::string> gpsData;
        std::vector<std::string> result;
        
        int gps_t;
        double lat;
        std::string ulat;
        double lon;
        std::string ulon;
        std::string numSv;
        std::string msl;

        std::string utctime;
        std::string cogt;
        std::string cogm;
        std::string sog;
        std::string kph;
};

extern getGPS GG;