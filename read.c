/*
 * shm-client - client program to demonstrate shared memory.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define SHMSZ     20
#define PAGESIZE 40
main()
{
    int shmid,pageCount;
    key_t key;
    char *pages, *blocked;
    
    key = 5678;

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    
    if ((pages = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat1");
        exit(1);
    }
    
    if ((shmid = shmget(key+1, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    
    if ((blocked = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat1");
        exit(1);
    }
    pageCount=atoi(pages);
    printf("Total de paginas %d", pageCount);
    
    while(1){
        
        printf("Actual buscando memoria: \n");
        printf(pages+PAGESIZE);
        printf("\n");
        
        printf("Paginas: \n");
        for(int page =2; page<=pageCount+1;page++){
            printf(pages+(PAGESIZE*page));
            printf("\n");
        }
        
        printf("Bloqueadas: \n");
        for(int page =0; page<20;page++){
            if((blocked+(32*page))[0]!='A'){
              printf(blocked+(32*page));
              printf("\n");  
            }
        }
        sleep(5);
    }
    exit(0);
}
