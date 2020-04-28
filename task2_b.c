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
#include <sys/wait.h>

#include <semaphore.h>

#define SHARED_MEMORY "/sharedmemory"
#define CRIT_REG1 "/critreg1"
#define CRIT_REG2 "/critreg2"


const char* BASE_PATH = "/home/thomas/Temp/";

int main()
{
    /* code */
    int fd = shm_open(SHARED_MEMORY,O_RDWR,0777);
    if(fd == -1){
        perror("shm_open");
        return EXIT_FAILURE;
    }


    
    int *buffer = (int *)mmap(0, sizeof(uint64_t), PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);
    int n = buffer[0];

    sem_t *reg1 = sem_open(CRIT_REG1,O_CREAT|O_EXCL,0777,0);
    sem_t *reg2 = sem_open(CRIT_REG2,O_CREAT|O_EXCL,0777,1);

    int child1 = fork();
    if(child1 == -1){
        perror("fork");
        return EXIT_FAILURE;
    }

    if(child1 > 0){
        //parent
        int child2 = fork();
        if(child2 == -1){
            perror("fork");
            return EXIT_FAILURE;
        }
        if(child2 > 0){
            
            while(wait(0)>0);

            FILE *fp_fifo;

            char* str = malloc(sizeof(BASE_PATH)+sizeof("pipe"));
            strcat(str,BASE_PATH);
            strcat(str,"pipe");

            if ( (fp_fifo = fopen(str, "w")) == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            char* buf = malloc(sizeof(char)*1024);
            sprintf(buf,"%d",buffer[10]);
            printf("Return value %s\n",buf);
            fprintf(fp_fifo, "%s", buf);
            free(buf);


            if ( fclose(fp_fifo) == EOF ) {
                perror("fclose");
                exit(EXIT_FAILURE);
            }
            sem_close(reg1);
            sem_close(reg2);
            sem_unlink(CRIT_REG1);
            sem_unlink(CRIT_REG2);

        }
        else{
            //child2
            int result = 0;
            for (int i = 0; i < n; i++)
            {
                sem_wait(reg1);
                result += buffer[i % 10];
                printf("Consumer: %d\n",buffer[i % 10]);
                sem_post(reg2);
            }
            buffer[10] = result;
        }

    }
    else{
        //child1
        for (int i = 0; i < n; i++)
        {
            sem_wait(reg2);
            buffer[i % 10] = i+1;
            printf("Producer: %d\n",buffer[i % 10]);
            sem_post(reg1);
            
        }
    }

    return EXIT_SUCCESS;
}
