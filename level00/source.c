#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int32_t main(int32_t argc, char** argv, char** envp)
{
    puts("***********************************");
    puts("* \t     -Level00 -\t\t  *");
    puts("***********************************");
    printf("Password:");
    int32_t var_14;
    __isoc99_scanf("%d", &var_14);
    
    if (var_14 != 0x149c)
    {
        puts("\nInvalid Password!");
        return 1;
    }
    
    puts("\nAuthenticated!");
    system("/bin/sh");
    return 0;
}