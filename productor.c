#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>


#define SHMSZ     27
#define PAGESIZE 256

int* fit;//1 worst 2 best 3 first
*fit=0;
   
int shmid;
key_t key=5678;
char *pages,*line,*lineBuffer;
int pageCount;

typedef struct proceso{
	int duracion;
	int tamano;
	int id;
};

void setPage(int i, char* text){
    strcpy((*(pages+PAGESIZE*i)), text);
}

char* getPage(int page){
    if(page>=pageCount){return 'O';}
    return pages+(PAGESIZE*page);
}

int randomInt(int min, int max){
    return (rand()%(max-min +1)) + min;
}




int bestFit(int size){
    int found=0;
    int best=pageCount+1;
    int result=-2;
    for(int i=0;i<pageCount+1;i++){
        if(getPage(i)[0]=='A'){
            found++;
        }else{
            if(found>=size && found<best){
                best=found;
                result=i-found;
            }
            found=0;
        }
    }
    return result;
}

int worstFit(int size){
    int found=0;
    int best=0;
    int result=-2;
    for(int i=0;i<pageCount+1;i++){
        if(getPage(i)[0]=='A'){
            found++;
        }else{
            if(found>=size && found>best){
                best=found;
                result=i-found;
            }
            found=0;
        }
    }
    return result;
}

int firstFit(int size){
    int found=0;
    int result=-2;
    for(int i=0;i<pageCount+1;i++){
        if(getPage(i)[0]=='A'){
            found++;
            if(found==size){return i-found;}
        }else{
            found=0;
        }
    }
    return result;
}



void *procesar(void *input){
    struct proceso *pr=input;
	struct proceso p=*pr;
    char strID[12];
    char available[12]="Available";
    printf("Thread %d con duracion %d\n",p.id,p.duracion);
    int space;
    
    switch(*fit){
        case 1:space = worstFit(p.tamano);break;
        case 2:space = bestFit(p.tamano);break;
        case 3:space = firstFit(p.tamano);break;
        default: break;
    }
    if(space != -2){
      sprintf(strID, "%d", p.id);
      char str[30] = "Ocupied by Process: ";
      strcat(str,strID);
      for(int i=0; i<p.tamano;i++){
          setPage(space+i,str);
      }
      sleep(p.duracion);
      for(int i=0; i<p.tamano;i++){
          setPage(space+i,available);
      }
      printf("Thread %d termino\n",p.id);
    }
    else{ printf("Thread %d no encontro espacio\n",p.id);}
    pthread_exit(NULL);
    
}




int main(){
    srand(time(NULL));
    while(fit!=1 && fit!=2 && fit!=3){
        printf("Algoritmo a utilizar:\n");
        printf("1. Worst fit\n");
        printf("2. Best fit\n");
        printf("3. First fit\n");
        scanf("%d", &fit);
    }
    printf("Escogio: %d",fit);
    
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((pages = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    pageCount=atoi(pages);
    pages=pages+PAGESIZE;
    
    int id=0;
    while(1){
        printf("ejecutando\n");
        struct proceso *p=malloc(sizeof(*p));
        p->duracion=randomInt(20,60);
        p->tamano=randomInt(1,10);
        p->id = id;
        id=id+1;
        pthread_t thread;
        pthread_create(&thread, NULL, procesar, p);
        pthread_detach(thread);
        sleep(randomInt(1,2));//cambiar a lo que diga shumman
    }
}
//
