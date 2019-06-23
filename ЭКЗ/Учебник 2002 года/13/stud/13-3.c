#include <signal.h>

int main(void){

    (void)signal(SIGINT, SIG_IGN);
    while(1);
    return 0;

}