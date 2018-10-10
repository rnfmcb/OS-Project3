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


#define KEY  849047   /* Parent and child key for shared memory */
#define BUFF_SZ sizeof ( int )

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


int main(int argc, char *argv[]){
int shmid = shmget ( KEY, BUFF_SZ, 0777 );

    if (shmid == -1 ){
      perror("Failed seconds in child"); 
      exit (1);
    }
clock = (int *)shmat(shmid,NULL,0); 
shmMsg = (struct message *)shmat(shmid,NULL,0);   
 int *cint = (int *)(shmat (shmid, 0, 0 ) );
	printf("something in child/n");


return 0; 
}


