#ifndef JEU_H
#define JEU_H

// Méthode pour générer les carte aléatoirement
void generer_cartes(int *cartes, int nb_cartes);

// Méthode pour mélanger les cartes
void melanger_cartes(int *cartes, int nb_cartes);

// Méthode pour vérifier si les cartes sont dans le bon ordre
int verifier_ordre_cartes(int *cartes, int nb_cartes);

// Méthode pour démarer une nouvelle manche
void demarrer_manche(int *cartes, int nb_cartes);

// Méthode pour finir une manche (réussie ou échouée)
int finir_manche(int *cartes_jouees, int nb_cartes);

// Méthode pour afficher les cartes
void afficher_cartes(int *cartes, int nb_cartes);

#endif