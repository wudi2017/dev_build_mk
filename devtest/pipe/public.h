#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <sys/types.h>  
#include <sys/wait.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
  
#define MAXLINE 512  
#define ERR_EXIT(sz) \  
        do{ \  
            perror(sz); \  
            exit(EXIT_FAILURE); \  
        }while(0)  
  
const char* reply_pipe = "/tmp/reply_pipe";  
const char* request_pipe = "/tmp/request_pipe";