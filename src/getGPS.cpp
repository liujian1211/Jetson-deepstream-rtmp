#include "getGPS.h"
#include <sstream>
#include <iomanip>
#include <mutex>
// #include "deepstream_test.h"

getGPS GG;

int getGPS::gpsOpen()
{
    int serialPort = open("/dev/ttyUSB0",O_RDWR);
    if (serialPort<0)
    {
        std::cerr<<"打开GPS串口失败"<<std::endl;
        return -1;
    }
    else
    {
        std::cout<<"打开GPS串口成功"<<std::endl;
        // std::cout<<"GPS端口为"<<serialPort<<std::endl;
    }
 
    memset(&tty, 0, sizeof(tty));

    if (tcgetattr(serialPort, &tty) != 0) 
    {
        std::cerr << "Error getting GPS attributes" << std::endl;
        return -1;
    }

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    //其它串口设置
    tty.c_cflag |= (CLOCAL | CREAD); // Enable receiver, ignore modem control lines
    tty.c_cflag |= PARENB;  // No parity
    tty.c_cflag &= ~CSTOPB; //1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      //8-bit数据
    tty.c_cflag &= ~CRTSCTS; // No hardware flow control
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    if (tcsetattr(serialPort, TCSANOW, &tty) != 0) 
    {
        std::cerr << "Error setting GPS attributes" << std::endl;
        return -1;
    }

    return serialPort;

}

// std::vector <std::string> getGPS::getGPSData(int serial_fd,const Callback& callback)
void getGPS::getGPSData(int serial_fd,const Callback& callback)
{
    
    char buffer[256];
    std::string gps_data;

    while(true)
    {      
        int bytes_read = read(serial_fd, buffer, sizeof(buffer));  // 从串口读取数据
        
        if (bytes_read > 0) 
        {
            // std::cout<<"bytes_read的值为"<<bytes_read<<std::endl;
            buffer[bytes_read] = '\0';   
            gps_data += buffer;
            size_t pos = gps_data.find("\n");
            if (pos != std::string::npos) 
            {
                std::string line = gps_data.substr(0, pos);
                // std::cout << "GPS data: " << line << std::endl;
                // 解析GPS数据，提取经纬度、海拔、方向等信息
                
                std::string substr_gngga;
                std::string substr_gnvtg;

                std::size_t GNGGA = line.find("$GNGGA");
                if (GNGGA != std::string::npos) 
                {
                    substr_gngga = line.substr(GNGGA + 6);  // 从"$GNGGA"后面开始截取子字符串
                    // std::cout << "Substring after $GNGGA: " << substr_gngga << std::endl;
                    std::vector<std::string> vec_gngga = GG.parseLine(substr_gngga);
                    if(vec_gngga.size()<19)
                    {
                        std::cout<<"GPS not found"<<std::endl;
                        GG.gps_t=0;
                    }
                    else
                    {
                        GG.utctime=vec_gngga[0];
                        GG.lat = GG.convert2Degrees(vec_gngga[2],vec_gngga[3]);
                        GG.ulat = vec_gngga[4];
                        GG.lon = GG.convert2Degrees(vec_gngga[5],vec_gngga[6]);
                        GG.ulon = vec_gngga[7];
                        GG.numSv = vec_gngga[9];
                        GG.msl = vec_gngga[12] + "." + vec_gngga[13] + vec_gngga[14];
                        GG.gps_t=1;

                        // std::cout<<"utctime的值为"<<GG.utctime<<std::endl;
                        // std::cout<<"lat的值为"<<GG.lat<<std::endl;
                        // std::cout<<"ulat的值为"<<GG.ulat<<std::endl;
                        // std::cout<<"lon的值为"<<GG.lon<<std::endl;
                        // std::cout<<"ulon的值为"<<GG.ulon<<std::endl;
                        // std::cout<<"numSv的值为"<<GG.numSv<<std::endl;
                        // std::cout<<"msl的值为"<<GG.msl<<std::endl;                       
                    }
            
                }
                std::size_t GNVTG = line.find("$GNVTG");
                if (GNVTG != std::string::npos)
                {
                    substr_gnvtg = line.substr(GNVTG + 6);  // 从"$GNVTG"后面开始截取子字符串
                    // std::cout << "Substring after $GNVTG: " << substr_gnvtg << std::endl;
                    std::vector<std::string> vec_gnvtg = GG.parseLine(substr_gnvtg);
                    GG.cogt = vec_gnvtg[0] + '.'+vec_gnvtg[1] + "T";
                    if(vec_gnvtg[3]=="M")
                    {
                        GG.cogm = "0.00";
                        GG.sog = vec_gnvtg[4] + vec_gnvtg[5];
                        GG.kph =  vec_gnvtg[7] + vec_gnvtg[8];
                    }
                    else if(vec_gnvtg[3]!="M")
                    {
                        GG.cogm = vec_gnvtg[3] + vec_gnvtg[4];
                        GG.sog = vec_gnvtg[6] + vec_gnvtg[7];
                        GG.kph =  vec_gnvtg[9] + vec_gnvtg[10];
                    }
                    
                    // std::cout<<"cogm的值为"<<GG.cogm<<std::endl;
                    // std::cout<<"sog的值为"<<GG.sog<<std::endl;
                    // std::cout<<"kph的值为"<<GG.kph<<std::endl;
                    // std::cout<<"vec_gnvtg的size为"<<vec_gnvtg.size()<<std::endl;

                }
                
                gps_data = gps_data.substr(pos + 1);
            }
            
            result.clear();
            result.push_back(GG.utctime);
            result.push_back(std::to_string(GG.lat) + GG.ulat);
            result.push_back(std::to_string(GG.lon) + GG.ulon);
            result.push_back(GG.numSv);
            result.push_back(GG.msl);
            result.push_back(GG.cogt+"°");
            result.push_back(GG.cogm+"°");
            result.push_back(GG.sog + "Kn");
            result.push_back(GG.kph+"km/h");

            // GG.gpsData = result;
            
        }       
        callback(result);
    }
}


//回调函数，用以在getGPSData中实时得到最后的result
std::vector<std::string> handleGPSData(const std::vector<std::string>&data) 
{
    // for (int i=0;i<data.size();i++)
    // std::cout<<data[i]<<"__";
    //     std::cout<<std::endl;
    return data;
}

double getGPS::convert2Degrees(std::string in_data1,std::string in_data2)
{
    int len_data1 = in_data1.length();
    int str_data2 = std::stoi(in_data2); //前置添加0
    int temp_data = std::stoi(in_data1);
    int symbol = 1;

    if (temp_data < 0) 
    {
        symbol = -1;
    }
    
    int degree = int(temp_data / 100.0);
    std::string str_decimal = std::string(1, in_data1[len_data1-2]) + in_data1[len_data1-1] + std::to_string(str_data2);
    double f_degree = std::stoi(str_decimal) / 60.0 / 100000.0;

    double result;
    if (symbol > 0) {
        result = degree + f_degree;
    } else {
        result = degree - f_degree;
    }
    // std::cout<<"result为"<<result<<std::endl;
    
    return result;
}


std::vector <std::string> getGPS::parseLine(std::string input)
{
    std::stringstream ss(input);
    std::string token;
    std::vector<std::string> result;

    while (std::getline(ss, token, ',')) 
    {
        std::string inner_token;
        std::stringstream inner_ss(token);
        while(std::getline(inner_ss,inner_token,'.'))
        {
            std::string innermost_token;
            std::stringstream innermost_ss(inner_token);
            while(std::getline(innermost_ss,innermost_token,'*'))
            {
                result.push_back(innermost_token);
            }
        }
    }

    return result;
}

// int main()
// {   
    
//     bool executed = false;
    
//     getGPS GG;
//     int serial_port = GG.gpsOpen();
    
//     GG.getGPSData(serial_port,[&executed](std::vector<std::string>result)
//     {
       
//         for(const auto& data : result)
//         {
//             std::cout << data << "--"; 
//         }   
//         std::cout<<std::endl;        
       
//     });
    
    // std::vector<std::string>result = GG.getGPSData(serial_port,handleGPSData);
    // GG.getGPSData(serial_port,[&result](const std::vector<std::string>&data)
    // {
    //     handleGPSData(data,result);
        // if(result.size())
        // {
        //     std::cout<<"经度为"<< result[1]<<std::endl;
        //     std::cout<<"纬度为"<< result[2]<<std::endl;
        //     std::cout<<"海拔为"<< result[4]<<std::endl;
        //     std::cout<<"真北朝向为"<<result[5]<<std::endl;  
        //     std::cout<<"速度为"<<result[8]<<std::endl;
        //     std::cout<<std::endl;
        // }
    //     else
    //     {
    //         std::cout<<"GPSdata为空"<<std::endl;
    //     }
    // });
    

    // while(true)
    // {
    //     std::vector<std::string> gpsdata = GG.getGPSData(serial_port);
    //     if(gpsdata.size())
    //     {
    //         std::cout<<"*******************************"<<std::endl;
    //         std::cout<<"UTC time:"<<gpsdata[0]<<std::endl;
    //         std::cout<<"Latitude:"<<gpsdata[1]<<std::endl;
    //         std::cout<<"Longtitude:"<<gpsdata[2]<<std::endl;
    //         std::cout<<"Number of satellites:"<<gpsdata[3]<<std::endl;
    //         std::cout<<"Altitude:"<<gpsdata[4]<<std::endl;
    //         std::cout<<"True north heading:"<<gpsdata[5]<<std::endl;
    //         std::cout<<"Magnetic north heading:"<<gpsdata[6]<<std::endl;
    //         std::cout<<"Ground speed(kn):"<<gpsdata[7]<<std::endl;
    //         std::cout<<"Ground speed(kph):"<<gpsdata[8]<<std::endl;
    //         std::cout<<"*******************************"<<std::endl;
    //         std::this_thread::sleep_for(std::chrono::seconds(1));
    //     }
    //     else
    //         break;     
    // }
    
/*******************************************************************************/
    // int serial_fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);  // 打开串口设备
    // if (serial_fd == -1) {
    //     std::cerr << "Error opening serial port" << std::endl;
    //     return 1;
    // }else
    // {
    //     std::cout<<"打开GPS串口成功"<<std::endl;
    // }

    // struct termios options;
    // tcgetattr(serial_fd, &options);
    // cfsetispeed(&options, B9600);  // 设置波特率为9600
    // cfsetospeed(&options, B9600);
    // options.c_cflag &= ~PARENB;    // 无校验位
    // options.c_cflag &= ~CSTOPB;    // 1个停止位
    // options.c_cflag &= ~CSIZE;
    // options.c_cflag |= CS8;        // 8位数据位
    // options.c_cflag |= (CLOCAL | CREAD);   // 本地连接，接收使能
    // tcsetattr(serial_fd, TCSANOW, &options);  //serial_fd的值为3
    
    // char buffer[256];
    // std::string gps_data;
    // while (true) {
    //     int bytes_read = read(serial_fd, buffer, sizeof(buffer));  // 从串口读取数据
    //     if (bytes_read > 0) {
    //         buffer[bytes_read] = '\0';
    //         gps_data += buffer;
    //         size_t pos = gps_data.find("\n");
    //         if (pos != std::string::npos) 
    //         {
    //             std::string line = gps_data.substr(0, pos);
    //             // 解析GPS数据，提取经纬度、海拔、方向等信息
    //             std::cout << "GPS data: " << line << std::endl;
                
    //             std::string substr_gngga;
    //             std::string substr_gnvtg;

    //             std::size_t GNGGA = line.find("$GNGGA");
    //             if (GNGGA != std::string::npos) 
    //             {
    //                 substr_gngga = line.substr(GNGGA + 6);  // 从"$GNGGA"后面开始截取子字符串
    //                 // std::cout << "Substring after $GNGGA: " << substr_gngga << std::endl;
    //                 std::vector<std::string> vec_gngga = GG.parseLine(substr_gngga);
    //                 // std::cout<<"vec_gngga的size为"<<vec_gngga.size()<<std::endl;
    //                 if(vec_gngga.size()<19)
    //                 {
    //                     std::cout<<"GPS not found"<<std::endl;
    //                     GG.gps_t=0;
    //                 }
    //                 else
    //                 {
    //                     GG.utctime=vec_gngga[0];
    //                     GG.lat = GG.convert2Degrees(vec_gngga[2],vec_gngga[3]);
    //                     GG.ulat = vec_gngga[4];
    //                     GG.lon = GG.convert2Degrees(vec_gngga[5],vec_gngga[6]);
    //                     GG.ulon = vec_gngga[7];
    //                     GG.numSv = vec_gngga[9];
    //                     GG.msl = vec_gngga[12] + "." + vec_gngga[13] + vec_gngga[14];
    //                     GG.gps_t=1;
    //                     // std::cout<<"utctime的值为"<<GG.utctime<<std::endl;
    //                     // std::cout<<"lat的值为"<<GG.lat<<std::endl;
    //                     // std::cout<<"ulat的值为"<<GG.ulat<<std::endl;
    //                     // std::cout<<"lon的值为"<<GG.lon<<std::endl;
    //                     // std::cout<<"ulon的值为"<<GG.ulon<<std::endl;
    //                     // std::cout<<"numSv的值为"<<GG.numSv<<std::endl;
    //                     // std::cout<<"msl的值为"<<GG.msl<<std::endl;                       
    //                 }
            
  
    //             }
    //             std::size_t GNVTG = line.find("$GNVTG");
    //             if (GNVTG != std::string::npos)
    //             {
    //                 substr_gnvtg = line.substr(GNVTG + 6);  // 从"$GNVTG"后面开始截取子字符串
    //                 // std::cout << "Substring after $GNVTG: " << substr_gnvtg << std::endl;
    //                 std::vector<std::string> vec_gnvtg = GG.parseLine(substr_gnvtg);
    //                 GG.cogt = vec_gnvtg[0] + '.'+vec_gnvtg[1] + "T";
    //                 if(vec_gnvtg[3]=="M")
    //                 {
    //                     GG.cogm = "0.00";
    //                     GG.sog = vec_gnvtg[4] + vec_gnvtg[5];
    //                     GG.kph =  vec_gnvtg[7] + vec_gnvtg[8];
    //                 }
    //                 else if(vec_gnvtg[3]!="M")
    //                 {
    //                     GG.cogm = vec_gnvtg[3] + vec_gnvtg[4];
    //                     GG.sog = vec_gnvtg[6] + vec_gnvtg[7];
    //                     GG.kph =  vec_gnvtg[9] + vec_gnvtg[10];
    //                 }
                  
    //                 // std::cout<<"cogm的值为"<<GG.cogm<<std::endl;
    //                 // std::cout<<"sog的值为"<<GG.sog<<std::endl;
    //                 // std::cout<<"kph的值为"<<GG.kph<<std::endl;
    //                 // std::cout<<"vec_gnvtg的size为"<<vec_gnvtg.size()<<std::endl;

    //             }
    //             // std::cout << "GPS data: " << line << std::endl;
    //             gps_data = gps_data.substr(pos + 1);
    //         }
    //     }
    // }

    // close(serial_fd);  // 关闭串口
    // return 0;

// }