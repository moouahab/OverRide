# Analyse de la fonction `auth` du challenge level06

## ✨ Objectif

Comprendre le fonctionnement de la fonction `auth` présente dans le challenge level06. Cette fonction vérifie un identifiant ("login") et un entier ("serial") en utilisant un algorithme de transformation personnalisé.

---

## 🤓 Rôle de la fonction `auth`

La fonction `auth(char* login, int serial)` ne fait **pas** une véritable "sérialisation". Elle effectue plutôt un **calcul mathématique sur le login** afin de vérifier que le serial fourni correspond au résultat attendu.

> C'est donc un algorithme de **génération de clé personnalisée** ou un **hash customisé**.

---

## 🔢 Déroulement de la fonction `auth`

1. **Nettoyage de l'entrée** :

   * Supprime le caractère `\n` final du login (avec `strcspn`)

2. **Contrôle de longueur** :

   * Si la longueur est ≤ 5 caractères, échec automatique

3. **Anti-debug** :

   * Appel à `ptrace()` pour détecter un débogueur (facultatif pour nos tests)

4. **Initialisation du hash** :

   ```c
   hash = (login[3] ^ 0x1337) + 0x5eeded;
   ```

   * Le caractère **4ème** (index 3) du login est crucial !

5. **Boucle principale** sur chaque caractère du login :

   ```c
   for (i = 0; i < longueur; i++) {
       if (login[i] <= 0x1F)
           return 1; // caractère non imprimable

       hash += (login[i] ^ hash) % 0x539;
   }
   ```

6. **Vérification finale** :

   ```c
   return (hash == serial) ? 0 : 1;
   ```

---

## ⚖️ Interprétation mathématique

* Le `hash` commence par une **valeur dérivée du 4ème caractère**
* Chaque caractère suivant est **mêlé au hash** via un `XOR`, puis réduit par modulo
* Le hash évolue donc à chaque étape de manière non linéaire

Ce qui rend l'algorithme **non réversible directement** → on ne peut pas trouver un login à partir d'un serial sans bruteforce.

---

## ✅ Code de test en C

```c
int32_t compute_hash(const char *input) {
    size_t len = strnlen(input, 0x20);
    if (len <= 5) return 0;

    int32_t hash = ((unsigned char)input[3] ^ 0x1337) + 0x5eeded;
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = (unsigned char)input[i];
        hash += (int32_t)((c ^ hash) % 0x539);
    }
    return hash;
}
```

Ce code permet de **générer le serial attendue à partir d'un login**.

---

## 🚀 Applications possibles

* 🔒 Créer un keygen pour automatiser la génération de serials
* ⚠️ Tenter de retrouver un login à partir d'un serial donné (mais cela nécessite du bruteforce)
* 🐞 Bypasser `auth()` dans un exécutable patché

---

## 🎉 Conclusion

Ce challenge montre bien comment un programme peut utiliser un **algorithme maison** pour lier une entrée (login) à une clé (serial). Ce n'est pas de la "sérialisation" classique, mais bien un **mécanisme de validation** par transformation.
