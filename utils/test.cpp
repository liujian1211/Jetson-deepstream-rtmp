std::vector <std::string> getGPS::getGPSData(int serial_fd,const Callback& callback)
{
    std::vector <std::string>result;
    while(true)
    {
        result.push_back("GPS数据");
        callback(result);
    }
}

void sendLocation() 
{
    int serial_port = gpsOpen();
    getGPSData(serial_port,[&sockfd, &ssLength,&lastOutputTime](std::vector<std::string>result)
    {
      
    })
}

int message()
{
    std::thread sendLocationThread(sendLocation,sockfd);
}

// deepstream.h
class DeepStream
{
    public:
        static void data(std::string data1,std::string data2);
        static std::string lat;
        static std::string lon;
}

// deepstream.cpp
std::string DeepStream::lat;
std::string DeepStream::lon;
void DeepStream::data(std::string data1,std::string data2)
{     
    DeepStream::lat = data1;
    DeepStream::lon = data2;  
}

int DeepStream::deepstream_func()
{
    std::cout<<"date里的lat的值为"<<DeepStream::lat<<std::endl;
    std::cout<<"date里的lon的值为"<<DeepStream::lon<<std::endl;
}

// tcp_ip_message.cpp
void sendLocation(int sockfd) 
{
    DeepStream::data("data1","data2");
}

// deepstream.cpp
static std::string lat;
static std::string lon;
void DeepStream::data(std::string data1,std::string data2)
{       
    lat = data1;
    lon = data2;  
    std::cout<<"date里的lat的值为"<<lat<<std::endl;
    std::cout<<"date里的lon的值为"<<lon<<std::endl;
}

int DeepStream::deepstream_func()
{
  std::cout<<"lat的值为"<<lat<<std::endl;
  std::cout<<"lon的值为"<<lon<<std::endl;
}






cmake语句add_executable(target  src/deepstream_test.cpp)
可以生成target，在终端中输入LD_PRELOAD=/home/nano/liujian/test_websocket/utils/libmyplugins.so ./target可以运行target，
现要在tcp_ip_message.cpp中调用deepstream_test.cpp，通过以下cmake语句
add_executable(tcp_ip_message src/tcp_ip_message.cpp
                                src/deepstream_test.cpp)
生成tcp_ip_message，请问程序中该如何改变，使其能表示LD_PRELOAD=/home/nano/liujian/test_websocket/utils/libmyplugins.so