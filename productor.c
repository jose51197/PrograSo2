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
#include <time.h>

#define SHMSZ     27
#define PAGESIZE 40
void test(){printf("test\n");}

int fit;//1 worst 2 best 3 first
fit=0;
   
int shmid;
key_t key=5678;
char *pages,*line,*lineBuffer,*blocked;
int pageCount;
pthread_mutex_t table_mutex;
pthread_mutex_t blocked_mutex;
pthread_mutex_t file_mutex;
FILE * log;


typedef struct proceso{
	int duracion;
	int tamano;
	int id;
    int bPos;
};

void setPage(int i, char* text){
    strcpy( pages + PAGESIZE*i , text);
}

int setBlocked(int id){
    char strID[12];
    sprintf(strID, "%d", id);
    for(int i=0;i<20;i++){
        if((blocked+(i*32))[0]=='A'){
            strcpy(blocked+(i*32),strID);
            return i;
        }
    }
    return -1;
}

char* getPage(int page){
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
    test();
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
    time_t t = time(NULL);
    struct tm tm ;
    struct proceso *pr=input;
	struct proceso p=*pr;
    char strID[12];
    char available[12]="Available";
    printf("Thread %d con duracion %d\n",p.id,p.duracion);
    int space=-2;
    pthread_mutex_lock(&blocked_mutex);
    p.bPos=setBlocked(p.id);
    pthread_mutex_unlock(&blocked_mutex);
    pthread_mutex_lock(&table_mutex);
    strcpy(blocked+(p.bPos*32),"Available");
    sprintf(strID, "%d", p.id);
    setPage(-1,strID);
    sleep(2);
    switch(fit){
        
        case 1:space = worstFit(p.tamano);break;
        case 2:space = bestFit(p.tamano);break;
        case 3:space = firstFit(p.tamano);break;
        default: break;
    }
    if(space != -2){//si hay espacio
      char str[20] = "Ocupied ID: ";
      strcat(str,strID);
      strcat(str," Size: ");
      sprintf(strID, "%d", p.tamano);
      strcat(str,strID);
      strcat(str," Time: ");
      sprintf(strID, "%d", p.duracion);
      strcat(str,strID);
      for(int i=0; i<p.tamano;i++){
          setPage(space+i,str);
      }
      setPage(-1,"None");
      pthread_mutex_unlock(&table_mutex);
      tm = *localtime(&t);
      log= fopen("log.txt","a");
      fprintf (log,"Thread %d entro de la linea %d a la linea %d date: %d-%d-%d %d:%d:%d \n", p.id, space,space+p.tamano,tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
      fclose (log);
      sleep(p.duracion);
      for(int i=0; i<p.tamano;i++){
          setPage(space+i,available);
      }
      tm = *localtime(&t);
      log= fopen("log.txt","a");
      fprintf (log,"Thread %d desocupo su espacio de la linea %d a la linea %d date: %d-%d-%d %d:%d:%d \n", p.id, space,space+p.tamano,tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
      fclose (log);
      printf("Thread %d termino\n",p.id);
    }
    else{
       setPage(-1,"None");
      pthread_mutex_unlock(&table_mutex);
      tm = *localtime(&t);
      log= fopen("log.txt","a");
      fprintf (log,"Thread %d No logro encontrar espacio  date: %d-%d-%d %d:%d:%d \n", p.id, space,space+p.tamano,tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
      fclose (log);
        printf("Thread %d no encontro espacio\n",p.id);
        
    }
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
    printf("Escogio: %d \n",fit);
    
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget1");
        exit(1);
    }
    if ((pages = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat1");
        exit(1);
    }
    pageCount=atoi(pages);
    pages=pages+PAGESIZE*2;
    int id=0;
    if ((shmid = shmget(key+1, 20, 0666)) < 0) {
        perror("shmget1");
        exit(1);
    }
    if ((blocked = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat1");
        exit(1);
    }
    while(1){
        printf("Ejecutando\n");
        struct proceso *p=malloc(sizeof(*p));
        p->duracion=randomInt(20,60);
        p->tamano=randomInt(1,10);
        p->id = id;
        id=id+1;
        pthread_t thread;
        pthread_create(&thread, NULL, procesar, p);
        pthread_detach(thread);
        sleep(randomInt(30,60));//cambiar a lo que diga shumman
    }
}
//
