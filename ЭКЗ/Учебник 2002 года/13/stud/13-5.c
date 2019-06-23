#include <signal.h>
#include <stdio.h>

int i=0;

void (*p)(int);

void my_handler(int nsig){

   printf("Receive signal %d, CTRL-C pressed\n", nsig);
   i = i+1;
   if(i == 5) (void)signal(SIGINT, p);

}

int main(void){

    p = signal(SIGINT, my_handler);
    while(1);
    return 0;

}