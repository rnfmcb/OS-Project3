#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <ctype.h> 
#include <math.h> 
#define BUFF_SZ 2*sizeof ( int )

int randomNumber();
//void  deleteMemory();
  
struct message{
  char msg[550];
  int second;
  int millisecond;
  int userPid;
};


int *timeclock;
struct message* shmMsg;
int shmid; 

int main(int argc, char *argv[]){
int randomclock = 0; 

//Get values from parent 
shmid = atoi(argv[0]); 
printf("%d shmid in child",shmid); 
key_t key = atoi(argv[1]);  
printf("%s key in child",key); 

//Allocate memory 
 shmid = shmget (key, BUFF_SZ, 0777 );
    if (shmid == -1 ){
      perror("Failed shmget in child"); 
      exit (1);
    }
//Attaches to shared memory 

timeclock = (int *)shmat(shmid,NULL,0); 
shmMsg = (struct message *)shmat(shmid,NULL,0);   
     if (*timeclock ==(int) -1) { 
	perror("failed shmat in child");
        exit(1); 
     }  
randomclock  = randomNumber(); 
printf("Current time in child is %d:%d\n", timeclock[0],timeclock[1]); 
printf("Random number is %d \n", randomclock); 
//deleteMemory(); 
return 0; 
}

int randomNumber(){ 
    int num = 0; 
    int max = 1000000; 
    int min = 1; 
    //Seed the time 
    srand(time(0)); 
    num = (rand() % (max - min + 1)) + min; 
    return num; 
} 

/*void deleteMemory(){ 
    //Detach from memory 
    shmdt(timeclock);
    shmdt(shmMsg);

    /*delete  from shared memory*/
/*	if((shmctl(shmid, IPC_RMID, NULL)) == -1){ 
    		perror("Couldn't detach memory ");
  	}
} */ 	
