#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    printf(" === TEST DES MÉTHODE DE JEU ===");

    printf("Méthode << generer_cartes >>\n");
    Carte pile1[NB_CARTES_TOTAL];
    generer_cartes(pile1);
    afficher_cartes(pile1, NB_CARTES_TOTAL);

    printf("\n Méthode << melanger_cartes >>\n");
    Carte pile2[4] = {{3, 0}, {5, 0}, {7, 0}, {29, 0}};
    melanger_cartes(pile2, 4);
    afficher_cartes(pile2, 4);

    printf("\n Méthode << initialiser_joueur >>\n");
    Joueur joueur;
    initialiser_joueur(&joueur, 1);
    printf("ID joueur : %d, Nombre de cartes :%d\n", joueur.id, joueur.nb_cartes);
    if(joueur.main == NULL){ // Donc l'initialisation est correcte
        printf("La main du joueur est vide :3\n");
    }

    return 0;
}