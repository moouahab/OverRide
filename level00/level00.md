# Exploitation — Level00

## Objectif

Obtenir un shell en passant la vérification de mot de passe dans le binaire `level00`.

## 1) Résumé rapide

Le programme lit un entier depuis l'entrée standard, le compare à une constante (`0x149c`). Si la valeur est correcte, il appelle `system("/bin/sh")` — on obtient donc un shell.

## 2) Vérification des protections

Exécuter :

```bash
checksec --file ./level00
```

Attendu : `Partial RELRO`, `No canary found`, `NX disabled`, `No PIE`.

Conclusion : le binaire n’a pas de protections fortes (pas de canary, NX désactivé, pas de PIE) — exploitation simple possible.

## 3) Analyse rapide du `main` (désassemblage)

Extrait pertinent :

```asm
8b 44 24 1c    mov 0x1c(%esp),%eax      ; récupérer la valeur lue
3d 9c 14 00 00 cmp $0x149c,%eax        ; comparer avec 0x149c
75 1f          jne <fail>
...            puts("Authenticated!")
...            system("/bin/sh")
```

Le programme lit un entier (via `scanf`) et compare `eax` à `0x149c`.

## 4) Conversion hex → décimal

```bash
python3 -c 'print(0x149c)'
```

Résultat : `5276`

## 5) Exploitation (étapes)

1. Lancer le binaire :

```bash
./level00
```

2. Entrer le nombre attendu :

```
5276
```

3. Si correct, le programme affiche `Authenticated!` et exécute `/bin/sh`.

## 6) Démo (exemple de session)

```
$ ./level00
************************************
*            -Level00-             *
************************************
Password: 5276
Authenticated!
$ whoami
level01
```


## 7) Remarques de sécurité / bonnes pratiques

* Ne pas exécuter de binaires non fiables en tant que root sur une machine de production.
* Ici, l’absence de NX et l’utilisation directe de `system()` rendent le binaire vulnérable : en conditions réelles, éviter `system()` et valider strictement les entrées.


---
