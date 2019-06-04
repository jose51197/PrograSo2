#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
//ipcrm -M 5678
//ipcs -m
#define SHMSZ    16384
#define SHMSZ2   20

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
    char (*shm)[size+3][40];
    char (*shmB)[20][32];

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
    strcpy((*shm)[1], "None");
    for(int i=2;i<=size;i++){
        strcpy((*shm)[i], "Available");
    }
    strcpy((*shm)[size+1], "Fin");
    key = 5679;

    if ((shmid = shmget(key, SHMSZ2, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }


    if ((shmB = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    for(int i=0;i<20;i++){
        strcpy((*shmB)[i], "Available");
    }
    printf("Shared memory done \n");
    exit(0);
}
