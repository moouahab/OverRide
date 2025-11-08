#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char **argv, char **envp) {
    char username[0x64];     /* 0x64 = 100 bytes */
    char file_pass[0x30];    /* 0x30 = 48 bytes */
    char password[0x40];     /* 0x40 = 64 bytes */


    memset(username, 0, sizeof(username));
    memset(file_pass, 0, sizeof(file_pass));
    memset(password, 0, sizeof(password));

    FILE *fp = fopen("/home/users/level03/.pass", "r");
    if (!fp) {
        fwrite("ERROR: failed to open password file\n", 1, 36, stderr);
        exit(1);
    }
    size_t r = fread(file_pass, 1, 0x29, fp);
    if (r != 0x29) {
        fwrite("ERROR: failed to read password file\n", 1, 36, stderr);
        fclose(fp);
        exit(1);
    }
    file_pass[r] = '\0';
    fclose(fp);
    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");

    printf("--[ Username: ");
    if (fgets(username, sizeof(username), stdin) == NULL) {
        fprintf(stderr, "ERROR: failed to read username\n");
        exit(1);
    }
    username[strcspn(username, "\n")] = '\0';

    /* demander le password */
    printf("--[ Password: ");
    if (fgets(password, sizeof(password), stdin) == NULL) {
        fprintf(stderr, "ERROR: failed to read password\n");
        exit(1);
    }
    password[strcspn(password, "\n")] = '\0';

    puts("*****************************************");

    if (strncmp(file_pass, password, 0x29) == 0) {
        printf("Greetings, %s!\n", username);
        system("/bin/sh");
        return 0;
    }

    /* affichage sécurisé en cas d'échec */
    printf("%s does not have access!\n", username);
    exit(1);
}
