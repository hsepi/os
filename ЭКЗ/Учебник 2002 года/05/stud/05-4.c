#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
   int     fd, result;

   size_t  size;
   char    resstring[14];
   char    name[] = "aaa.fifo";

   if(mknod(name, S_IFIFO | 0666, 0) < 0){
     printf("Can\'t create FIFO\n");
     exit(-1);
   }

   if((result = fork()) < 0){

      printf("Can\t fork child\n");
      exit(-1);

   } else if (result > 0) {

     /* Parent process */

      if((fd = open(name, O_WRONLY)) < 0){
         printf("Can\'t open FIFO for writting\n");
	 exit(-1);
      }

      size = write(fd, "Hello, world!", 14);

      if(size != 14){
        printf("Can\'t write all string to FIFO\n");
        exit(-1);
      }

      close(fd);
      printf("Parent exit\n");

   } else {

      /* Child process */

      if((fd = open(name, O_RDONLY)) < 0){
         printf("Can\'t open FIFO for reading\n");
	 exit(-1);
      }

      size = read(fd, resstring, 14);

      if(size < 0){
         printf("Can\'t read string from FIFO\n");
         exit(-1);
      }

      printf("Child exit, resstring:%s\n", resstring);

      close(fd);
   }
   return 0;
}
