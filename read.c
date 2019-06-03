/*
 * shm-client - client program to demonstrate shared memory.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define SHMSZ     27
#define PAGESIZE 32
main()
{
    int shmid,pageCount;
    key_t key;
    char *pages, *s,*sAux;
    /*
     * We need to get the segment named
     * "5678", created by the server.
     */
    key = 5678;

    /*
     * Locate the segment.
     */
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    
    if ((pages = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat1");
        exit(1);
    }
    pageCount=atoi(pages);
    printf("Total de paginas %d", pageCount);
    
    /*
     * Now read what the server put in the memory.
     */
    while(1){
        printf("Paginas: \n");
        for(int page =1; page<=pageCount+1;page++){
            printf(pages+(PAGESIZE*page));
            printf("\n");
        }
        sleep(5);
    }
    exit(0);
}
