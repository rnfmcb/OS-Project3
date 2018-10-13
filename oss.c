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
#define BUFF_SZ	sizeof ( int )
#define KEY 8419047

void master(pid_t);
void deleteMemory();
void updateClock();

struct message{ 
char msg[550]; 
int second; 
int millisecond; 
int userPid; 
};

 
int *clock; 
int shmid; 
char *filename;   
struct message* shmMsg;
int processIds[100];  
int processNum = 0; 
void printlog(char *, struct message shmMsg, int *); 

int main(int argc, char *argv[]){ 
int x = 5; 
int z = 2; 
int  c;    
struct message msg1; 

while ((c = getopt (argc, argv, "hslt:")) != -1)
switch (c) {
        case 'h':
		printf("s is the maximum number of user processes spawned");
		printf("\n l is the logfile name"); 
		printf("\n t is the time in seconds before the processes are killed");  
	 	 break; 
        case 's':
		x = atoi(argv[2]);  
     	  	printf("The max number of processes spawned is %d/n", x);   
       	        break; 
        case 'l':
                filename = optarg; 
	   	break; 
	 case 't':
		z = atoi(argv[2]); 
		printf("The total amount of time for processes is %d/n",z); 
		break; 	   
         default: 
		abort(); 
      } 
//Allocate shared memory 
shmid = shmget(KEY, BUFF_SZ, IPC_CREAT | 0777);
   if(shmid == -1){
        perror("Error in shmget");
        exit(1);
   }
//Attach shared memory 
     clock = (int *)shmat(shmid,NULL,0);
     shmMsg = (struct message *)shmat(shmid,NULL,0);
//Initialize the clock to 0 
     clock[0] = 0;
     clock[1] = 0;

//Fork the user processes 
int i = 0; 
while ( i < x){ 

pid_t childPid;
int status = 0;
switch ( childPid = fork() ){
	case -1:
	    perror("Error in forking"); 
	    return (1);

	case 0:
	   // child();
	   execlp("./user","./user",(char *)NULL);
	   fprintf(stderr,"%s failed to exec user!\n",argv[0]);
		exit(-1);
	    break;

	default:
	    master(childPid); 
	    break;
    }
i++; 
}

deleteMemory(); 

return 0; 
}

void master(pid_t childPid) {
     int returnStatus; //Wait for child process
     waitpid(childPid,&returnStatus,0); 
     printf("Something done in the master\n"); 
     updateClock(); 
     
  

//int returnStatus;//Wait for child process
//waitpid(childPid, &returnStatus,0);
 
//printlog(&filename, msg1, &clock;) 
}

//Detach and remove the memory
void deleteMemory(){
   shmdt(clock);
   shmdt(shmMsg);
      if((shmctl(shmid,IPC_RMID,NULL))== -1)
       perror("Error deleting memory");

}
void updateClock() { 

   //Add values to  the milliseconds 
    
   int mili = clock[1] + 100; 
   int sec = clock[0]; 
   int overflow = 0; 
   int seconds = 0; 
   //Check for overflow 
   if (mili >= 1000) { 
       seconds = mili / 1000; 
      clock[0] = sec + seconds;  //Add overflow of seconds + current seconds 
      overflow = mili % 1000;   //left over miliseconds 
   } 
  else{ 
     clock[0] = sec + seconds; 
     clock[1] = mili; 
   }  
     
   printf("Clock has been updated to %d:%d\n",clock[0],clock[1]); 

} 

void clean (int sig){ 
    printf("Removing shared memory"); 
    int i;
    shmdt(clock);
     shmdt(shmMsg);
      if((shmctl(shmid,IPC_RMID,NULL))== -1)
       perror("Error deleting memory");

	for(i = 0; i < processNum; i++){
		kill(processIds[i], SIGKILL);
	}
	exit(1);
} 



   
   /* shmdt(secptr);
    perror("Failed to detatch");  
    shmctl(secid, IPC_RMID, NULL);
    perror("Failed to delete memory"); 
*/


/*void printlog (char *filename, struct message* shmMsg, int *clock){
   FILE *fpointer = fopen(filename, "w");
   fprintf(fpointer, "OSS: %d is terminating at my time %d.%d because it reached %d.%d in user.\n",shmMsg[0].userPid, clock[0], clock[1], shmMsg[0].second, shmMsg[0].millisecond);
}
*/
