# Le Jeu "The Mind"

## Aperçu

**The Mind** est un jeu de cartes coopératif où les joueurs doivent jouer leurs cartes dans l'ordre croissant sans communiquer directement.

---

## Prérequis

Avant de lancer le jeu, assurez-vous d'avoir les outils suivants installés sur votre système :
- Un terminal ou une interface en ligne de commande. 
- L'outil `make`.
- Un compilateur C (comme GCC).

---

## Lancer le jeu

### Étapes pour exécuter le programme

1. **Naviguez jusqu'au dossier du projet** :
    ```bash
    cd chemin/vers/le/dossier_du_projet
    ```

2. **Compilez le projet** :
    ```bash
    make
    ```

3. **Ouvrez trois terminaux** :
    - Tous les terminaux doivent pointer vers le même dossier du projet.

4. **Démarrez le serveur** :
    ```bash
    ./serveur adresse_ip
    ```
    - Exemple : `./serveur 127.0.0.1` pour jouer en local.
  
    - **Si vous voulez jouer sur des machines différentes :**
      
      Exécutez la commande suivante sur la machine où le serveur est lancé pour récupérer son adresse ip :
      ```bash
      hostname -I | awk '{print $1}'
      ```
      


5. **Lancez un joueur humain** :
    ```bash
    ./joueur adresse_ip nom_joueur
    ```
    - Remplacez `adresse_ip` par l'adresse IP du serveur.
    - Remplacez `nom_joueur` par le nom du joueur.

6. **Lancez un joueur robot** :
    ```bash
    ./robot adresse_ip
    ```
    - Par défaut, le robot s'appelle **chatGPT**.

---

## Notes importantes

- Assurez-vous que tous les terminaux utilisent le même répertoire de projet.
- Le serveur doit être démarré avant de connecter des joueurs ou des robots.

---

## Résolution des problèmes

- **Problème** : Impossible de se connecter au serveur.  
  **Solution** : Vérifiez que le serveur est bien lancé et que l'`adresse_ip` est correcte.  

- **Problème** : Erreur de compilation.  
  **Solution** : Vérifiez que `make` et un compilateur C sont installés sur votre système.  

---

## Exemples de commandes

- Pour démarrer le serveur en local :
    ```bash
    ./serveur 127.0.0.1
    ```

- Pour lancer un joueur nommé Alice :
    ```bash
    ./joueur 127.0.0.1 Alice
    ```

- Pour lancer un robot :
    ```bash
    ./robot 127.0.0.1
    ```

---

## Licence

Ce projet est sous licence [MIT](LICENSE).  

---
