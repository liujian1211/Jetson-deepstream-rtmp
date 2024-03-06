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
using namespace LibSerial;

class getGPS
{
    private:
        struct termios tty;
        char data;
        // std::vector <std::string> result;

        char buffer_[256];

    public:
      
        getGPS();
        ~getGPS();
        int gpsOpen();

        using Callback = std::function<void(const std::vector<std::string>&)>;
        std::vector <std::string> getGPSData(int serial_fd, const Callback& callback);       

        double convert2Degrees(std::string in_data1,std::string in_data2);
        std::vector <std::string> parseLine(std::string input);
        
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