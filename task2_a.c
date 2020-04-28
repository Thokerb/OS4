#include <stdlib.h>
#include <stdio.h> 
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/types.h>
#include <sys/select.h>

#include <stdint.h> 


#define SHARED_MEMORY "/sharedmemory"

const char* BASE_PATH = "/home/thomas/Temp/";

void makeFifoPipe(const char* fifoName){
    char* str = malloc(sizeof(BASE_PATH)+sizeof(fifoName));
    strcat(str,BASE_PATH);
    strcat(str,fifoName);
    printf("making pipe %s\t",str);
    if ( mkfifo(str, 0777) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    printf("created pipe %s\n",str);
}

int openFifoPipe(const char* fifoName){
    int fp_fifo = 0;

    char* str = malloc(sizeof(BASE_PATH)+sizeof(fifoName));
    strcat(str,BASE_PATH);
    strcat(str,fifoName);
    printf("opening connection for %s\t",str);

    if ( (fp_fifo = open(str, O_RDONLY)) == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("%s connected\n",fifoName);
    return fp_fifo;
}



int main(int argc, char const *argv[])
{
    /* code */
    if(argc != 2){
        printf("wrong number of parameters");
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);

    
    int fileDescriptor = shm_open(SHARED_MEMORY,O_CREAT | O_RDWR,0777);
    if(fileDescriptor == -1){
        perror("shm_open");
        return EXIT_FAILURE;
    }
    if(ftruncate(fileDescriptor,sizeof(uint64_t)) == -1){
        perror("ftruncate");
        return EXIT_FAILURE;
    }

    int *data = mmap(NULL,sizeof(uint64_t),PROT_READ | PROT_WRITE,MAP_SHARED,fileDescriptor,0); 
    if(data == MAP_FAILED){
        perror("mmap");
        return EXIT_FAILURE;
    }

    data[0] = n;

    fd_set connections;
    FD_ZERO(&connections);

    makeFifoPipe("pipe");
    int conn = openFifoPipe("pipe");
    FD_SET(conn,&connections);

    int openConn = 0;
    char* buf = malloc(sizeof(char)*1024);
    

    while (1)
    {
        openConn = select(FD_SETSIZE,&connections,NULL,NULL,NULL);
        if(openConn == -1){
            perror("select");
            exit(EXIT_FAILURE);
        }

        if(FD_ISSET(conn,&connections)){
            read(conn,buf,sizeof(buf));
            printf("%s",buf);
            free(buf);
            break;
        }
    }
    close(conn);

    char* str = malloc(sizeof(BASE_PATH)+sizeof("pipe"));
    strcat(str,BASE_PATH);
    strcat(str,"pipe");
    unlink(str);
    return EXIT_SUCCESS;
}
