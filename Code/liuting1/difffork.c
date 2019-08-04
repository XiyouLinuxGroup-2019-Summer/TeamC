/*************************************************************************
	> File Name: difffork.c
	> Author: 
	> Mail: 
	> Created Time: 2019年08月03日 星期六 09时44分02秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int globVar = 5;

int main()
{
    pid_t pid;
    int var = 1;
    int i;

    printf("fork is different with vfork");
    //不打印换行符打印两遍，


    pid = fork();
    //pid = vfork();
    
    switch (pid)
    {
        case 0:
        {
            i = 3;
            while (i--)
            {
                printf ("Child process is running\n");
                globVar++;
                var++;
                sleep(1);
            }
            printf ("Child‘s globVar = %d, var = %d\n",globVar,var);
            break;
        }
        case -1:
        {
            perror ("Process creation failed\n");
            exit(0);
        }
        default:
        {
            i=5;
            while (i--)
            {
                printf("Parent process is running\n");
                globVar++;
                var++;
                sleep(1);
            }
            printf ("Parent's globVar = %d, var =%d\n",globVar,var);
            exit(0);
        }
    }
}
