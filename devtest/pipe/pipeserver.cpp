#include "public.h"  

int main()  
{  
    int res;  
    if(access(request_pipe, F_OK) < 0){     //检测是否存在  
        if( (res = mkfifo(reply_pipe, O_CREAT|O_EXCL|0755)) < 0)   //创建写fifo，0755为执行权限  
            printf("mkfifo err.\n");  
    }  
      
    int write_fd;  
    if( (write_fd = open(reply_pipe, O_WRONLY)) < 0){  
        unlink(reply_pipe);            //如果失败，删除  
        printf("open reply_pipe err.\n");  
    }  
      
    printf("wait client.\n");  
    int request_fd;  
    while( (request_fd = open(request_pipe, O_RDONLY)) < 0)   //等待客户端创建读fifo  
        sleep(1);  
    printf("client connect.\n");  
  
    char sendbuff[MAXLINE];  
    char recvbuff[MAXLINE];  
      
    for(; ;){  
        printf("client:>");  
        ssize_t ret;  
        if( (ret = read(request_fd, recvbuff, MAXLINE)) < 0)  
            printf("read err.\n");  
        printf("%s\n", recvbuff);  
          
        printf("server:>");  
        scanf("%s", sendbuff);  
        write(write_fd, sendbuff, strlen(sendbuff)+1);  
    }  
    unlink(reply_pipe);  
  
    return 0;  
}  