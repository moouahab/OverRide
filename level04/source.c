#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>
#include <errno.h>

int main(int argc, char **argv, char **envp)
{
    pid_t eax = fork();
    char buf[128];
    memset(&buf, 0, sizeof(buf));
    int32_t var_18 = 0;
    int stat_loc = 0;

    if (eax > 0)
    {
        while (1)
        {
            wait(&stat_loc);
            if (!(stat_loc & 127) || ( (((stat_loc & 127) + 1) >> 1) > 0) )
            {
                puts("child is exiting...");
                break;
            }

            errno = 0;
            long val = ptrace(PTRACE_PEEKUSER, eax, (void *)44, 0);
            if (val == -1 && errno != 0) {
                continue;
            }

            if (val != 11)
                continue;
            puts("no exec() for you");
            kill(eax, 9);
            break;
        }
    }
    else if (eax == 0)
    {
        prctl(1, 1);
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        puts("Give me some shellcode, k");
        gets(&buf);
    }
    else
    {
        perror("fork");
        return 1;
    }

    return 0;
}

