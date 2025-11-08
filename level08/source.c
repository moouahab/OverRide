#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

int64_t log_wrapper(FILE* arg1, char* arg2, char* arg3)
{
    uintptr_t fsbase = 0;
    int64_t rax = 0;
    char var_118[0x108];

    strcpy(var_118, arg2);
    int64_t i = -1;
    char* rdi_1 = var_118;
    
    while (i)
    {
        bool cond_0_1 = 0 != *rdi_1;
        rdi_1 = rdi_1 + 1;
        i -= 1;
        
        if (!cond_0_1)
            break;
    }
    
    int64_t i_1 = -1;
    char* rdi_2 = var_118;
    
    while (i_1)
    {
        bool cond_1_1 = 0 != *rdi_2;
        rdi_2 = rdi_2 + 1;
        i_1 -= 1;
        
        if (!cond_1_1)
            break;
    }
    
    snprintf(&var_118[~i_1 - 1], 0xfe - (~i - 1), "%s", arg3);
    var_118[strcspn(var_118, "\n")] = 0;

    fprintf(arg1, "LOG: %s\n", var_118);
    
    int64_t result = rax ^ fsbase;
    
    if (!result)
        return result;
    
    __stack_chk_fail();
    return 0;
}



int32_t main(int32_t argc, char** argv, char** envp)
{
    uintptr_t fsbase = 0;
    int64_t rax = 0;
    unsigned char buf = 0xff;
    int32_t var_80 = 0xffffffff;
    
    if (argc != 2)
    {
        printf("Usage: %s filename\n", argv[0]);
        return 1;
    }
    
    FILE* rax_4 = fopen("./backups/.log", "w");
    
    if (!rax_4)
    {
        printf("ERROR: Failed to open %s\n", "./backups/.log");
        exit(1);
    }
    
    log_wrapper(rax_4, "Starting back up: ", argv[1]);
    FILE* fp = fopen(argv[1], "r");
    
    if (!fp)
    {
        printf("ERROR: Failed to open %s\n", argv[1]);
        exit(1);
    }
    
    char file[0x64];
    strncpy(file, "./backups/", 0xb);
    int64_t i = -1;
    char* rdi_2 = file;
    
    while (i)
    {
        bool cond_0_1 = 0 != *rdi_2;
        rdi_2 += 1;
        i -= 1;
        
        if (!cond_0_1)
            break;
    }
    
    strncat(file, argv[1], 0x63 - (~i - 1));
    int32_t fd = open(file, O_WRONLY | O_CREAT | O_EXCL, 0660);
    
    if (fd < 0)
    {
        printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
        exit(1);
    }
    
    while (true)
    {
        int c = fgetc(fp);
        
        if (c == EOF)
            break;
        
        buf = (unsigned char)c;
        write(fd, &buf, 1);
    }
    
    log_wrapper(rax_4, "Finished back up ", argv[1]);
    fclose(fp);
    close(fd);
    
    if (rax == fsbase)
        return 0;
    
    __stack_chk_fail();
    return 0;
}
