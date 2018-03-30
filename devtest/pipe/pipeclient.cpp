#include "public.h"  

int main()  
{  
	printf("mkfifo %s.\n", request_pipe);  
    int res;  
    if( (res = mkfifo(request_pipe, O_CREAT|O_EXCL|0755)) < 0)  
    {
    	printf("mkfifo err.\n");  
    }
      
    printf("open %s.\n", reply_pipe);  
    int read_fd;  
    if( (read_fd = open(reply_pipe, O_RDONLY)) < 0)  
    {
        printf("open reply_pipe err.\n"); 
    }
  
  	printf("open %s.\n", request_pipe);  
    int request_fd;  
    if( (request_fd = open(request_pipe, O_WRONLY)) < 0){  
        unlink(request_pipe);  
        printf("open request_pipe err.\n");  
    }  
  
    char sendbuff[MAXLINE];  
    char recvbuff[MAXLINE];  
      
    ssize_t ret;  
    for(; ;){  
        printf("client:>");  
        scanf("%s", sendbuff);  
        if( (ret = write(request_fd, sendbuff, strlen(sendbuff)+1)) < 0)  
            printf("write err.\n");  
  
        printf("server:>");  
        read(read_fd, recvbuff, MAXLINE);  
        printf("%s\n", recvbuff);  
    }  
  
    unlink(request_pipe);  
  
    return 0;  
}  