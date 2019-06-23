#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[], char *envp[])
{
    int     shmid;
    sem_t  *sem;
    char    pathname[]="08-1a.c";
    key_t   key;
    int     new = 1;
    
    key = ftok(pathname, 0);
    
    if((shmid = shmget(1, sizeof(sem_t), 0666 | IPC_CREAT | IPC_EXCL)) < 0){
      if(errno != EEXIST){
         printf("Can\'t create shared memory\n");
         exit(-1);
      } else {
        if((shmid = shmget(1, sizeof(sem_t), 0)) < 0){
           printf("Can\'t create shared memory\n");
           exit(-1);
        } 
	new = 0;
      } 	  
    }
    
    if((sem = (sem_t *)shmat(shmid, NULL, 0)) == (sem_t *)(-1)){
      printf("Can\'t attach shared memory\n");
      exit(-1);
    }  
    
    if(new){  
      if(sem_init(sem, 1, 0) < 0){
        printf("Can\'t init semaphore\n"); exit(-1);
      }
    }
        
    if(sem_wait(sem)< 0){
      printf("Can\'t wait semaphore\n"); exit(-1);
    }
    
    printf("The condition is present\n");
    return 0;
}
