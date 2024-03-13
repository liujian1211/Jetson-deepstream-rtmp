#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

void sendHeartbeat(int sockfd);
void executeCommand(); //执行cmd命令 
void sendLocation(int sockfd);
int MessageType(int sockfd);

class tcpIpMessage
{
    private:

    public:
        int message();
    
        // using Callback = std::function<void(std::vector<std::string>&)>;
        // void updateLatiAndLon(std::string ret1,std::string ret2,const Callback& callback);
};
