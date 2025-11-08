#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

char a_user_name[0x100];

int32_t verify_user_name(void)
{
    const char *expected = "dat_wil";
    char *esi = a_user_name;
    int i = 7;
    int c = 0;
    int z = 0;

    puts("verifying username....\n");

    while (i)
    {
        unsigned char temp1 = (unsigned char)*esi;
        unsigned char temp2 = (unsigned char)*expected;
        c = (temp1 < temp2);
        z = (temp1 == temp2);
        esi++;
        expected++;
        i--;

        if (!z)
            break;
    }
    return (int32_t)((!z && !c) - c);
}

int32_t verify_user_pass(char* arg1)
{
    const char *expected = "admin";
    char *esi = arg1;
    int i = 5;
    int c = 0;
    int z = 0;

    while (i)
    {
        unsigned char temp0 = (unsigned char)*esi;
        unsigned char temp1 = (unsigned char)*expected;
        c = (temp0 < temp1);
        z = (temp0 == temp1);
        esi++;
        expected++;
        i--;

        if (!z)
            break;
    }

    int edx = (!z && !c);
    return (int32_t)(edx - c);
}

int32_t main(int32_t argc, char** argv, char** envp)
{
    char buf[0x64];
    memset(buf, 0, sizeof(buf));

    puts("********* ADMIN LOGIN PROMPT *********");
    printf("Enter Username: ");
    if (fgets(a_user_name, sizeof(a_user_name), stdin) == NULL) {
        return 1;
    }
    /* strip newline */
    a_user_name[strcspn(a_user_name, "\n")] = '\0';

    if (verify_user_name())
    {
        puts("nope, incorrect username...\n");
        return 1;
    }

    puts("Enter Password: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        return 1;
    }
    buf[strcspn(buf, "\n")] = '\0';

    int32_t eax_2 = verify_user_pass(buf);
    if (eax_2 && !eax_2)
        return 0;

    puts("nope, incorrect password...\n");
    return 1;
}
