#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * Version compilable qui garde le comportement original :
 * - lecture via fgets
 * - transformation A-Z -> a-z en XOR 0x20
 * - affichage via printf(var_78) (vulnérable format-string)
 *
 * UTILISATION DANGEREUSE : ne pas exécuter sur une machine de production.
 */

int32_t main(int32_t argc, char** argv, char** envp)
{
    (void)argc; (void)argv; (void)envp;
    char var_78[100];
    /* lire depuis stdin (remplace __bss_start) */
    if (fgets(var_78, sizeof(var_78), stdin) == NULL) {
        return 1;
    }
    for (size_t i = 0; var_78[i] != '\0' && i < sizeof(var_78); ++i) {
        unsigned char c = (unsigned char)var_78[i];
        if (c > 0x40 && c <= 0x5A) {
            var_78[i] = (char)(c ^ 0x20);
        }
    }
    printf(&var_78);
    exit(0);
}

