#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Générer les cartes aléatoirement
void generer_cartes(int *cartes, int nb_cartes){
    for(int i = 0; i < nb_cartes; i++){
        cartes[i] = i+1;
    }
}

// Mélanger les cartes
void melanger_cartes(int *cartes, int nb_cartes){
    srand(time(NULL));
    for(int i = 0; i < nb_cartes; i++){
        int j = rand() % nb_cartes;
        int temp = cartes[i];
        cartes[i] = cartes[j];
        cartes[j] = temp;
    }
}

// Vérifier si les cartes sont dans l'ordre croissant
int verifier_ordre_cartes(int *cartes, int nb_cartes){
    for(int i = 1; i < nb_cartes; i++){
        if(cartes[i] < cartes[i-1]){
            return 0; // Ordre incorrect
        }
    }
    return 1; // Les cartes sont dans le bon ordre
}

// Démarrer une nouvelle manche
void demarrer_manche(int *cartes, int nb_cartes){
    generer_cartes(cartes, nb_cartes);
    melanger_cartes(cartes, nb_cartes);
}

// Fin de la manche : réussie ou échouée
int finir_manche(int *cartes_jouees, int nb_cartes){
    return verifier_ordre_cartes(cartes_jouees, nb_cartes);
}

// Afficher les cartes
void afficher_cartes(int *cartes, int nb_cartes){
    for(int i = 0; i < nb_cartes; i++){
        printf("Carte %d: %d\n", (i+1), cartes[i]);
    }
}