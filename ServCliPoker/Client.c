#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>
 
int main(int argc,char **argv)
{
    int sockfd,n,running = 1;
    char sendline[100];
    char recvline[100];
    struct sockaddr_in servaddr;
 
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1)
    {
        printf("Failed to create socket!");
        return 1;
    }
    bzero(&servaddr,sizeof servaddr);
 
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(22000);
 
    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));
 
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
    {
        printf("Failed to connect to the server!");
        close(sockfd);
        return 1;
    }
    else
    {
        printf("Connected to the server!\n");
        printf("Enter your name: ");
        fgets(sendline,100,stdin);
        send(sockfd,sendline,strlen(sendline+1),0);
        read(sockfd,recvline,100);
        printf("\nServer: Witaj %s\n\n", recvline);
    }
    
    while(running)
    {
        bzero( sendline, 100);
        //bzero( recvline, 100);
        printf("Enter message: ");
        fgets(sendline,100,stdin);
        if(!strcmp("Stop\n",sendline)) {running = 0;}
        if(send(sockfd,sendline,strlen(sendline+1),0)<0)
        {
            printf("Failed to send!");
            return 1;
        }
    }
 close(sockfd);
}