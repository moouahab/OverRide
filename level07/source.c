#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

static int clear_stdin(void)
{
    int c;
    do {
        c = getchar();
        if (c == '\n' || c == EOF) break;
    } while (c != EOF);
    return c;
}

static int32_t get_unum(void)
{
    int32_t result = 0;
    fflush(stdout);
    if (scanf("%u", (unsigned int *)&result) != 1) {
        clear_stdin();
        return 0;
    }
    clear_stdin();
    return result;
}

int32_t store_number(int32_t *base)
{
    int32_t value, index;

    printf(" Number: ");
    value = get_unum();

    printf(" Index: ");
    index = get_unum();
    if ((index % 3) && ((unsigned int)value >> 24) != 0xb7U)
    {
        base[index] = value;
        return 0;
    }

    puts(" *** ERROR! ***");
    puts("   This index is reserved for wil!");
    puts(" *** ERROR! ***");
    return 1;
}

/* lit un nombre depuis la "heap" */
int32_t read_number(int32_t *base)
{
    int32_t index;
    printf(" Index: ");
    index = get_unum();

    /* affichage comme dans le pseudo */
    printf(" Number at data[%u] is %u\n", (unsigned int)index,
           (unsigned int)base[index]);
    return 0;
}

int main(int argc, char **argv, char **envp)
{
    (void)argc;

    int32_t data[100];
    memset(data, 0, sizeof(data));

    for (char **a = argv; a && *a; ++a) {
        size_t len = strlen(*a);
        memset(*a, 0, len);
    }


    for (char **e = envp; e && *e; ++e) {
        size_t len = strlen(*e);
        memset(*e, 0, len);
    }

    puts("----------------------------------------------------\n"
         "  Welcome to wil's crappy number storage service!\n"
         "----------------------------------------------------\n"
         " Commands:\n"
         "    store - store a number into the data storage\n"
         "    read  - read a number from the data storage\n"
         "    quit  - exit the program\n"
         "----------------------------------------------------");

    char buf[0x14]; /* 20 bytes */
    while (1)
    {
        printf("Input command: ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            puts("");
            break;
        }

        buf[strcspn(buf, "\n")] = '\0';

        int ret = 1;

        if (strcmp(buf, "store") == 0) {
            ret = store_number(data);
        } else if (strcmp(buf, "read") == 0) {
            ret = read_number(data);
        } else if (strcmp(buf, "quit") == 0) {
            return 0;
        } else {
            ret = 1;
        }
        if (!ret) {
            printf(" Completed %s command successfully\n", buf);
        } else {
            printf(" Failed to do %s command\n", buf);
        }
        memset(buf, 0, sizeof(buf));
    }

    return 0;
}
