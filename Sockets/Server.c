#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
 

int main()
{
    struct epoll_event event, events[10];
    int epoll_fd = epoll_create1(0);
    if(epoll_fd == -1)
    {
        printf("Failed to create epoll file descriptor!");
        return 1;
    }
    char str[100], sendLine[100];
    int listen_fd, comm_fd, running =1, event_count, i;
 
    struct sockaddr_in servaddr;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd == -1)
    {
        printf("Failed to create socket!");
        return 1;
    }  



    bzero( &servaddr, sizeof(servaddr));
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(22000);
 
    if(bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0)
    {
        printf("Failed to bind!");
        return 1;
    }
 
    listen(listen_fd, 10);
 
    comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
    if(comm_fd < 0)
    {
        printf("Failed to accept connection!");
    }
    
    event.events = EPOLLIN;
    event.data.fd = comm_fd;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, comm_fd, &event))
    {
        printf("Failed to add file descriptor to epoll\n");
        close(epoll_fd);
        return 1;
    }
    
    while(running)
    {
        printf("Waiting for input...\n");
        event_count = epoll_wait(epoll_fd, events, 10, 30000);
        printf("Ready events: %d\n", event_count);
        for(i = 0; i < event_count; i++)
        {
        bzero( str, 100);
        printf("Reading file descriptor '%d' --\n", events[i].data.fd);
        //read(comm_fd,str,100);
        recv(comm_fd,str,2000,0);
        if(!(strcmp(str,"stop\n"))){running = 0;}
        printf("Received message: %s\n",str);
        bzero( str, 100);
        
        //printf("Enter message: %s",str);
        //fgets(str,100,stdin);
        //write(comm_fd, str, strlen(str)+1);
      
        }
   }
if(close(epoll_fd))
{
    printf("Failed to close epoll file descriptor\n");
    return 1;
}
return 0;
}