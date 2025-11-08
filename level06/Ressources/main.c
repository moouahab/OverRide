// auth_test.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <stdint.h>

int32_t auth(char *input, int32_t arg2)
{
    input[strcspn(input, "\n")] = '\0';
    size_t len = strnlen(input, 0x20);
    if (len <= 5)
        return 1;
    int32_t hash = ( (unsigned char)input[3] ^ 0x1337 ) + 0x5eeded;
    for (size_t i = 0; i < len; ++i)
    {
        unsigned char c = (unsigned char)input[i];
        if (c <= 0x1F) return 1;
        hash += (int32_t)((c ^ hash) % 0x539);
    }
    return (hash == arg2) ? 0 : 1;
}

/* same hash function to compute expected arg2 */
int32_t compute_hash(const char *input)
{
    size_t len = strnlen(input, 0x20);
    if (len <= 5) return 0; // invalid length, but function used only for correct inputs

    int32_t hash = ( (unsigned char)input[3] ^ 0x1337 ) + 0x5eeded;
    for (size_t i = 0; i < len; ++i)
    {
        unsigned char c = (unsigned char)input[i];
        hash += (int32_t)((c ^ hash) % 0x539);
    }
    return hash;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <password>\n", argv[0]);
        return 2;
    }

    char input[128];
    strncpy(input, argv[1], sizeof(input)-1);
    input[sizeof(input)-1] = '\0';

    int32_t expected = compute_hash(input);
    printf("Computed hash (arg2) = %d (0x%08x)\n", expected, (unsigned)expected);

    /* Call auth with computed correct arg2 -> should return 0 */
    int32_t res_ok = auth(input, expected);
    printf("auth(input, expected) returned: %d  (0 = success)\n", res_ok);

    /* Call auth with wrong arg2 -> should return 1 */
    int32_t res_bad = auth(input, expected + 1);
    printf("auth(input, expected+1) returned: %d  (1 = fail)\n", res_bad);

    return 0;
}
