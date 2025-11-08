#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <time.h>

static int32_t clear_stdin(void) {
    int c;
    do {
        c = getchar();
        if (c == '\n' || c == EOF) break;
    } while (c != EOF);
    return c;
}

static uint32_t get_unum(void) {
    uint32_t result = 0;
    fflush(stdout);
    (void)scanf("%u", &result);
    clear_stdin();
    return result;
}

__attribute__((noreturn))
static void prog_timeout(int sig) {
    (void)sig;
    _exit(1);
}

static int32_t enable_timeout_cons(void) {
    signal(SIGALRM, prog_timeout);
    return alarm(0x3c); /* 60s */
}

/* Retourne 0 si auth OK, 1 sinon */
static int32_t auth(char *login, uint32_t serial) {
    /* strip newline */
    login[strcspn(login, "\n")] = '\0';

    /* longueur <= 5 => refuse */
    size_t len = strnlen(login, 0x20);
    if (len <= 5) return 1;

    /* anti-debug: si déjà tracé, ptrace(TRACEME) retourne -1 */
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
        puts("\x1b[32m.---------------------------.");
        puts("\x1b[31m| !! TAMPERING DETECTED !!  |");
        puts("\x1b[32m'---------------------------'");
        return 1;
    }

    int32_t acc = ((unsigned char)login[3] ^ 0x1337) + 0x5eeded;

    for (size_t i = 0; i < len; i++) {
        unsigned char ch = (unsigned char)login[i];
        if (ch <= 0x1f) return 1;               /* refuse les contrôles */
        acc += ((int32_t)ch ^ acc) % 0x539;     /* mise à jour */
    }

    return (serial == (uint32_t)acc) ? 0 : 1;
}

int32_t main(int32_t argc, char **argv, char **envp) {
    (void)argc; (void)argv; (void)envp;

    enable_timeout_cons();

    char login[0x20] = {0};   /* 32 */
    uint32_t serial = 0;

    puts("***********************************");
    puts("*\t\tlevel06\t\t  *");
    puts("***********************************");
    printf("-> Enter Login: ");
    if (!fgets(login, sizeof(login), stdin)) return 1;

    puts("***********************************");
    puts("***** NEW ACCOUNT DETECTED ********");
    puts("***********************************");

    printf("-> Enter Serial: ");
    serial = get_unum();

    if (auth(login, serial)) {
        return 1;
    } else {
        puts("Authenticated!");
        system("/bin/sh");
        return 0;
    }
}
