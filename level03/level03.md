# Override Level03 — Writeup

## 🎯 Objectif

Obtenir un shell en exploitant un binaire ELF 32 bits (`level03`) via une analyse statique et dynamique.

---

## 🧩 Analyse du binaire

### 🔍 Dans `main()` :

```asm
movl $0x1337d00d, 0x4(%esp)
call  test
```

* La valeur `0x1337d00d` (soit **322424845** en décimal) est passée en **argument** à `test()`

### 🔁 Dans `test()` :

* Cette valeur est transmise à la fonction `decrypt()`

### 🧠 Dans `decrypt()` :

* Une chaîne chiffrée de 16 octets est stockée sur la stack
* Chaque octet est **XORé** avec la clé passée en argument
* Puis le résultat est comparé à "Congratulations!" en clair

---

## 🧠 Comment sait-on que c’est un `int` codé en dur ?

### ✅ 1. Instruction `mov` immédiate :

```asm
movl $0x1337d00d, 0x4(%esp)
```

* Le `$` montre que c’est une **valeur immédiate**, donc directement écrite dans le binaire
* Elle n’est ni calculée, ni lue dynamiquement → donc **codée en dur**

### ✅ 2. Pas de lecture mémoire avant

* Il n’y a pas d’accès à la mémoire ou à l’entrée utilisateur
* Aucun `scanf`, aucun `read`, aucun `mov` depuis un pointeur mémoire

### ✅ 3. Taille de 4 octets = `int`

* `0x1337d00d` tient sur 32 bits → correspond à un `int` classique en C

---

## 🔍 Pourquoi j'ai pensé à une `int`

En observant le binaire avec :

```bash
objdump -R level03
```

On obtient la table de relocalisation dynamique :

```
0804a018 R_386_JUMP_SLOT   system
```

Cela montre que la fonction `system` est **liée dynamiquement** et **appelée** dans le binaire.
Cela m’a mis sur la piste qu’un shell pouvait être déclenché **si une condition était remplie**.

Ensuite, j’ai observé que la valeur passée était `0x1337d00d` (322424845), ce qui ressemble à un `int` codé en dur, typique d’un contrôle.

Le fait qu’un shell (`system("/bin/sh")`) soit appelé **après une comparaison réussie** a renforcé cette idée :
la bonne valeur est sûrement **proche**, donc j’ai testé `322424845 - 18`.

---

## 🧪 Travail fait dans GDB

### 📌 1. Observation de la chaîne chiffrée dans la pile :

```bash
gdb ./level03
(gdb) break *0x8048660  # break dans decrypt
(gdb) run
(gdb) x/16xb $ebp-0x1d  # lecture des octets chiffrés
```

On récupère :

```
[0x51, 0x7d, 0x7c, 0x75, 0x60, 0x73, 0x66, 0x67,
 0x7e, 0x73, 0x66, 0x7b, 0x7d, 0x7c, 0x61, 0x33]
```

### 📌 2. Lecture de la chaîne claire comparée :

```bash
(gdb) x/s 0x80489c3
→ "Congratulations!"
(gdb) x/16xb 0x80489c3
→ [0x43, 0x6f, 0x6e, 0x67, 0x72, 0x61, 0x74, 0x75, ...]
```

### 📌 3. Calcul de la clé :

En observant :

```
0x51 ^ 0x43 = 0x12
```

Donc la clé XOR est `0x12` (18 en décimal)

### 📌 4. Déduction de la bonne entrée :

```bash
0x1337d00d - 0x12 = 0x1337cffb = 322424827
```

Testé ensuite directement dans le programme :

```bash
./level03
Password: 322424827
→ Shell ($)
```

---

## 🔐 Chiffrement XOR

Le chiffrement est de la forme :

```c
decrypted[i] = encrypted[i] ^ (key & 0xFF);
```

Mais lors de l’analyse dynamique :

* On observe que la **clé réellement utilisée est `0x12`**
* Or `0x1337d00d & 0xFF = 0x0d` ❌ donc ce n’est pas juste un AND
* En testant plusieurs entrées, on découvre que la **bonne entrée est `0x1337d00d - 0x12`**

---

## ✅ Exploit

```bash
./level03
Password: 322424827
$
```

Shell obtenu 🎉

---

## 💡 Conclusion

L’analyse du XOR + compréhension de la clé réelle permet de contourner la comparaison interne, déclencher un `system("/bin/sh")`, et obtenir un shell.

L’indice venait aussi du fait que la fonction `system` était **liée dynamiquement** — donc probablement utilisée **à condition que la comparaison soit réussie.**

---

## 🛠️ Outils

* `gdb`
* `x/s`, `x/xb`
* `objdump -R`
* Python (pour tester les XOR)
* Lecture du binaire en assembleur (objdump)

---

## ✍️ Auteur
Moouahab

