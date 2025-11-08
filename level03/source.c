#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int32_t clear_stdin(void)
{
    int c;
    do {
        c = getchar();
        if (c == '\n' || c == EOF)
            break;
    } while (c != EOF);
    return c;
}

uint32_t get_unum(void)
{
    uint32_t value = 0;
    fflush(stdout);
    if (scanf("%u", &value) != 1) {
        clear_stdin();
        return 0;
    }
    clear_stdin();
    return value;
}

__attribute__((noreturn)) void prog_timeout(void)
{
    _exit(1);
}

int32_t decrypt(unsigned char key)
{
    char enc[] = "Q}|u`sfg~sf{}|a3";
    size_t n = sizeof(enc) - 1;
    char buf[sizeof(enc)];
    if (n >= sizeof(buf)) {
        return -1;
    }

    for (size_t i = 0; i < n; ++i) {
        buf[i] = enc[i] ^ key;
    }
    buf[n] = '\0';

    const char target[] = "Congratulations!";

    if (strncmp(buf, target, sizeof(target) - 1) == 0) {
        system("/bin/sh");
        return 0;
    } else {
        puts("\nInvalid Password");
        return 1;
    }
}

int32_t test(uint32_t a, uint32_t b)
{
    int32_t diff = (int32_t)b - (int32_t)a;

    if ((diff >= 1 && diff <= 9) || (diff >= 16 && diff <= 21)) {
        return decrypt((unsigned char)diff);
    }

    return decrypt((unsigned char)(rand() & 0xff));
}

int main(int argc, char **argv, char **envp)
{
    (void)argc; (void)argv; (void)envp;

    /* initialisation */
    srand((unsigned int)time(NULL));

    puts("***********************************");
    puts("*\t\tlevel03\t\t**");
    puts("***********************************");
    printf("Password (enter two unsigned numbers):\n");

    printf("--[ First value: ");
    uint32_t v1 = get_unum();
    printf("--[ Second value: ");
    uint32_t v2 = get_unum();

    test(v1, v2);

    return 0;
}
