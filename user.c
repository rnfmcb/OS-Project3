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
#include <semaphore.h> 
#define BUFF_SZ 2*sizeof ( int )

  
struct message{
  char *msg;
  int second;
  int millisecond;
  int userPid;
};
int randomNumber(); 
void updateClock(int);
int criticalSection(int *, int *, int, struct message user);
void deleteMemory(); 

int *timeclock;
int *localClock; 
struct message* shmMsg;
int shmid; 
int randomclock = 0;
 
int main(int argc, char *argv[]){
struct message user; 

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

//Create the local clock for termination deadline  
 localClock[0] = timeclock[0]; 
 localClock[1] = timeclock[1];  

//Update the clock to random termination time 
updateClock(randomclock); 

//Critical section, keeps trying to put message in shared memory
int i = 0;  
while (i = 0){ 
  criticalSection(timeclock,localClock,shmid,user); 
}   
//Terminate process
deleteMemory();  
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

void updateClock(int random) {

   int mili = localClock[1] + random;
   int sec = localClock[0];
   int overflow = 0;
   int seconds = 0;

   if (mili >= 1000) {
       seconds = mili / 1000;
      localClock[0] = sec + seconds;
      overflow = mili % 1000;
   }
  else{
     localClock[0] = sec + seconds;
     localClock[1] = mili;
   }

   printf("Clock has been updated to %d:%d\n",localClock[0],localClock[1]);

}

//Critical section 
int criticalSection(int *timeclock, int *localClock, int shmid, struct message user ) { 
     //If time is up 
     if (timeclock[0] >= localClock[0] && timeclock[1] >= localClock[1]){
        //if shmsg is null 
           //Update struct in shared memory 
           user.msg = "Time is up, user is terminating and sending message"; 
           user.second = timeclock[0]; 
           user.millisecond = timeclock[1]; 
	   user.userPid = shmid;
           //cede process to other children  
           return 1;  
      }    
       else{ 
         //Cede process to other child  
         return 0;   
       } 
}                
 

void deleteMemory(){ 
    //Detach from memory 
    shmdt(timeclock);
    shmdt(shmMsg);

    /*delete  from shared memory*/
	if((shmctl(shmid, IPC_RMID, NULL)) == -1){ 
    		perror("Couldn't detach memory ");
  	}
}  	
