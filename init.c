#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
//ipcrm -M 5678
//ipcs -m
#define SHMSZ    16384

main()
{
    int size;
    char strSize[12];
    printf("Ingrese la cantidad de lineas: ");            
    scanf ("%d", &size);
    sprintf(strSize, "%d", size);
    char c;
    int shmid;
    key_t key;
    char (*shm)[size+2][32];

    key = 5678;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }


    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    printf("%d \n",shm);
    strcpy((*shm)[0], strSize);
    for(int i=1;i<=size;i++){
        strcpy((*shm)[i], "Available");
    }
    strcpy((*shm)[size+1], "Fin");
    printf("Shared memory done");
    exit(0);
}
