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

#define SHARED_MEMORY "/sharedmemory"

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

    int child1 = fork();
    if(child1 == -1){
        perror("fork");
        return EXIT_FAILURE;
    }

    if(child1 == 0){
        //parent
        int child2 = fork();
        if(child2 == -1){
            perror("fork");
            return EXIT_FAILURE;
        }
        if(child2 == 0){
            
            wait(NULL);
            FILE *fp_fifo;

            char* str = malloc(sizeof(BASE_PATH)+sizeof("pipe"));
            strcat(str,BASE_PATH);
            strcat(str,"pipe");

            if ( (fp_fifo = fopen(str, "w")) == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            fprintf(fp_fifo, "%i", buffer[10]);


            if ( fclose(fp_fifo) == EOF ) {
                perror("fclose");
                exit(EXIT_FAILURE);
            }

        }
        else{
            //child2
            int result = 0;
            for (int i = 0; i < n; i++)
            {
                result += buffer[i % 10];
            }
            buffer[10] = result;
        }

    }
    else{
        for (int i = 0; i < n; i++)
        {
            buffer[i % 10] = i+1;
            
        }
        //child1
    }


    return EXIT_SUCCESS;
}
