#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 

int main() 
{ 
    
    system("killall read");
    system("killall productor");
    
	// ftok to generate unique key 
	key_t key = 5678;

	// shmget returns an identifier in shmid 
	int shmid1 = shmget(key,16384,0666|IPC_CREAT); 
    int shmid2 = shmget(key+1,20,0666|IPC_CREAT); 
    
	// destroy the shared memory 
	shmctl(shmid1,IPC_RMID,NULL); 
    shmctl(shmid2,IPC_RMID,NULL); 
    
	return 0; 
} 
