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
#include <semaphore.h> 
#include <sys/sem.h> 
#include <wait.h> 
#define BUFF_SZ	2048
#define KEY 8419047
#define SEM_ID 3000 //semaphore id 

void deleteMemory();

struct message{ 
     char shmsg[550]; 
     int seconds; 
     int milisec; 
     int userPid; 
     int userSec; 
     int userMili; 
}; 

void updateClock(struct message *msg); 
void printLog(char *, struct message *msg);  


//int *clock; 
int shmid; 
char *filename;   
char *shm_address; 
//nt processIds[100];  
int processNum = 0; 


int main(int argc, char *argv[]){
 
int x = 5; 
int z = 2; 
int  c;    
struct message msg;
msg.seconds = 0; 
msg.milisec = 0;
msg.shmsg[0] = 0; 
msg.userPid = 0;   
 
//semaphore information 
int semSetId; 
union semun{ 
    int value; 
    struct semid_ds  *buf; 
    ushort *array; 
} sem_val;  
struct shmid_ds  shm_desc; 


//Get values from command line 
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
        perror("Error in shmget in main");
        exit(1);
   }
//Attach shared memory 
   /*  clock = (int *)shmat(shmid,NULL,0);
     shmMsg = (struct message *)shmat(shmid,NULL,0);
//Initialize the clock to 0 
     clock[0] = 0;
     clock[1] = 0;*/
shm_address = shmat(shmid, NULL, 0); 
    if (shmid == -1) { 
       perror("Error in shmat in main"); 
       exit(1); 
     } 


//initialize and create structures for semaphore
semSetId = semget(SEM_ID, 1, IPC_CREAT| 0600); 
   if (semSetId == -1) 
      perror("Semaphore semget"); 
sem_val.value = 1; 
int returnSystemCall = semctl(semSetId, 0, SETVAL,sem_val); 
    if (returnSystemCall == -1) 
      perror("Semaphore semctl error"); 

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
	    sleep(2);
            int returnStatus; //Wait for child process
            waitpid(childPid,&returnStatus,0);
            printf("Something done in the master\n");
            updateClock(&msg);
 
	    break;
    }
i++; 
}

deleteMemory(); 

return 0; 
}

void deleteMemory(){
   if(shmdt(shm_address) == -1)  
     perror("Problem with shmdt"); 
   if((shmctl(shmid,IPC_RMID,NULL))== -1)
       perror("Error deleting memory");

}
void updateClock(struct message *msg) { 

   //Add values to  the milliseconds 
   int mili = msg->milisec + 100; 
   int sec = msg->seconds; 
   int overflow = 0; 
   int seconds = 0; 
   //Check for overflow 
   if (mili >= 1000) { 
       seconds = mili / 1000; 
      msg->seconds  = sec + seconds;  //Add overflow of seconds + current seconds 
      overflow = mili % 1000;   //left over miliseconds 
   } 
  else{ 
     msg->seconds = sec + seconds; 
     msg->milisec  = mili; 
   }  
     
   printf("Clock has been updated to %d:%d\n",msg->seconds,msg->milisec); 

} 

void clean (int sig){ 
    printf("Removing shared memory"); 
    int i;
      if((shmctl(shmid,IPC_RMID,NULL))== -1)
       perror("Error deleting memory");

      /*	for(i = 0; i < processNum; i++){
		kill(processIds[i], SIGKILL);
	}*/
	exit(1);
} 
//Signal handler to catch Ctrl C 
void  INThandler(int sig){
  printf("Removing shared memory");
   deleteMemory(); 

    signal(sig, SIG_IGN);
          exit(0);
 }              

//Semlock 
void semLock(int semSetId){ 
struct sembuf sem_op; //Semaphore structure
//Wait on the semaphore unless value is non-negative 
sem_op.sem_num = 0; 
sem_op.sem_op = -1;  
sem_op.sem_flg = 0; 
semop(semSetId, &sem_op, 1); 
} 

//unlocks the semaphore 
void semUnlock(int semSetId){ 
struct sembuf sem_op; //Structure for semaphore 

//Semaphore signal which increases value by one. 
sem_op.sem_num = 0; 
sem_op.sem_op - 1; 
sem_op.sem_flg = 0; 
semop(semSetId, &sem_op, 1); 
} 

   
   /* shmdt(secptr);
    perror("Failed to detatch");  
    shmctl(secid, IPC_RMID, NULL);
    perror("Failed to delete memory"); 
*/


void printlog (char *filename, struct message *msg){
   FILE *fpointer = fopen(filename, "w");
   fprintf(fpointer, "OSS: %d is terminating at my time %d.%d because it reached %d.%d in user.\n",msg->userPid, msg->seconds,msg->milisec,msg->userSec,msg->userMili); 
   //Free up shared memory message 
   msg->shmsg[0] = 0; 
}

