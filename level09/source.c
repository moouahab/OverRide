#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

// Fonction cachée accessible via l'exploit
void secret_backdoor() {
    char buffer[128];
    fgets(buffer, sizeof(buffer), stdin);
    system(buffer);
}


// Structure simulée
typedef struct s_message 
{
    char text[140]; // offset 0x00
    char username[40]; // offset 0x8C
    int len; // offset 0xB4
} t_message;


// set_msg utilise strncpy avec t.len comme taille
void set_msg(t_message *t)
{
    char buffer[1024] = {0};
    puts(">: Msg @Unix-Dude");
    printf(">>: ");
    fgets(buffer, sizeof(buffer), stdin);
    strncpy(t->text, buffer, t->len); // t->len dépend du champ potentiellement corrompu
}


// set_username copie caractère par caractère avec un off-by-one
void set_username(t_message *t)
{
    char buffer[140] = {0};
    puts(">: Enter your username");
    printf(">>: ");
    fgets(buffer, sizeof(buffer), stdin);
    for (int i = 0; i <= 40; i++) { // → off-by-one ici !
        if (buffer[i] == '\0') break;
            *((char*)t + 0x8C + i) = buffer[i];
    }
    printf(">: Welcome, %s", t->username);
}


// Fonction principale qui enchaîne les étapes
void handle_msg()
{
    t_message msg;
    memset(&msg, 0, sizeof(t_message));
    msg.len = 140; // Valeur par défaut
    set_username(&msg);
    set_msg(&msg);
    puts(">: Msg sent!");
}


int main() {
    puts("--------------------------------------------");
    puts("| ~Welcome to l33t-m$n ~ v1337 |");
    puts("--------------------------------------------");
    handle_msg();
    return 0;
}