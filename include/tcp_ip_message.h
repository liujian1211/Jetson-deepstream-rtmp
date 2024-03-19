#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include "messageGenerate.h"
// #include "getGPS.h"
// #include "deepstream_test.h"

void sendHeartbeat(int sockfd);
void executeCommand(); //执行cmd命令 
void sendLocation(int sockfd);
int MessageType(int sockfd);

class tcpIpMessage
{
    private:

    public:
        int message();
        
};
