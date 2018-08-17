#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>

int main()
{
    ssize_t n;
    struct epoll_event event, events[10];
    int epoll_fd = epoll_create1(0);
    if(epoll_fd == -1)
    {
        printf("Failed to create epoll file descriptor!");
        return 1;
    }
    char str[100], answer[20] = "Message received!",conn_message[100] = "Witaj ",name[100];
    int listen_fd, comm_fd, running =1, event_count, i,sock_fd,temp,cliCounter=0, k;
    char arrOfFd[10][100];
    int ifUser[100] = {0};
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;
    socklen_t clilen;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd == -1)
    {
        printf("Failed to create socket!");
        return 1;
    }  

    temp=fcntl(listen_fd, F_GETFL);
    temp = temp | O_NONBLOCK;
    fcntl(listen_fd,F_SETFL,temp);

    
    event.events = EPOLLIN;
    event.data.fd = listen_fd;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event))
    {
        printf("Failed to add file descriptor to epoll\n");
        close(epoll_fd);
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
 /////

    printf("Waiting for clients to connect...\n");
    while(running)
    {   
        event_count = epoll_wait(epoll_fd, events, 10, 30000);
        //printf("Ready events: %d\n", event_count);
        for(i = 0; i < event_count; i++)
        {
            if(events[i].data.fd == listen_fd)
            {
                //printf("accept connection from %d\n", listen_fd);
                comm_fd = accept(listen_fd, (struct sockaddr*) &clientaddr, &clilen);
                if(comm_fd < 0)
                {
                    printf("Failed to accept connection!");
                }
                else
                {  
                    read(comm_fd,name,10);
                    printf("Connection from %s accepted! fd(%d)\n", name, comm_fd);
                    cliCounter++;
                    ifUser[comm_fd] = 1;
                    strcpy(arrOfFd[comm_fd], name);
                    send(comm_fd,name,strlen(name),0);
                    //bzero(name,100);
                    if(cliCounter > 0)
                    {
                        printf("\nConnected clients: ");
                        for(k = 0; k <100;k++)
                        {
                        if(ifUser[k] == 1) {printf(" %s",arrOfFd[k]);}
                        }
                        printf("\n");
                    }
                    printf("Waiting for input...\n");
                }
                ///////////
                temp=fcntl(listen_fd, F_GETFL);
                 temp = temp | O_NONBLOCK;
                fcntl(listen_fd,F_SETFL,temp);

                event.data.fd = comm_fd;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epoll_fd,EPOLL_CTL_ADD,comm_fd,&event);

            }
            else if (events[i].events & EPOLLIN)
            {
                if((sock_fd = events[i].data.fd) < 0) continue;
                if((n= read(sock_fd, str, 100))<0)
                {
                    if(errno == ECONNRESET)
                    {
                        close(sock_fd);
                        events[i].data.fd = -1;
                    }
                    else
                    {
                        printf("Readline error");
                    }
                }
                else if (n ==0 )
                {
                    close(sock_fd);
                    events[i].data.fd = -1;
                }
                printf("Received message from %s: %s\n",arrOfFd[events[i].data.fd],str);
                if(!strcmp("Stop",str)){
                    bzero(name,100);
                    cliCounter--;
                    ifUser[events[i].data.fd] = 0;
                    printf("%s left session\n", arrOfFd[events[i].data.fd]);
                    close(events[i].data.fd);
                }
                bzero(str,100);
                event.data.fd=sock_fd;
                event.events=EPOLLOUT | EPOLLET;
                epoll_ctl(epoll_fd, EPOLL_CTL_MOD,sock_fd,&event);
            }
            else if (events[i].events & EPOLLOUT)
            {
                sock_fd = events[i].data.fd;
                
                write(sock_fd,answer,n);
                printf("written data: %s\n",answer);

                event.data.fd=sock_fd;
                event.events= EPOLLIN | EPOLLET;
                epoll_ctl(epoll_fd, EPOLL_CTL_MOD,sock_fd,&event);
                if(cliCounter > 0)
                {
                    printf("\nConnected clients: ");
                    for(k = 0; k <100;k++)
                    {
                    if(ifUser[k] == 1) {printf(" %s",arrOfFd[k]);}
                    }
                    printf("\n");
                }
                printf("Waiting for input...\n");
            }      
        }
   }
if(close(epoll_fd))
{
    printf("Failed to close epoll file descriptor\n");
    return 1;
}
return 0;
}