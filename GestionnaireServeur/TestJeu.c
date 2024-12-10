#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    printf(" === TEST DES MÉTHODE DE JEU ===");

    printf("Méthode << generer_cartes >>\n");
    Carte pile1[NB_CARTES_TOTAL];
    generer_cartes(pile1);
    afficher_cartes(pile1, NB_CARTES_TOTAL);

    printf("\nMéthode << melanger_cartes >>\n");
    Carte pile2[4] = {{3, 0}, {5, 0}, {7, 0}, {29, 0}};
    melanger_cartes(pile2, 4);
    afficher_cartes(pile2, 4);


    printf("\nMéthode << initialiser_joueur >>\n");
    Joueur joueur;
    initialiser_joueur(&joueur, 1);
    printf("ID joueur : %d, Nombre de cartes :%d\n", joueur.id, joueur.nb_cartes);
    if(joueur.main == NULL){ // Donc l'initialisation est correcte
        printf("La main du joueur est vide :3\n");
    }

    printf("\nMéthode << initialiser_jeu >>\n");
    Etats_Jeu jeu;
    int nb_joueurs = 4, vies = 3, shurikens = 3;
    initialiser_jeu(&jeu, nb_joueurs, vies, shurikens);
    afficher_etat_jeu(&jeu);

    printf("\nMéthode << distribure_cartes >>\n");
    int pile_indice = 0;
    distribuer_cartes(&jeu, pile2, &pile_indice);
    afficher_etat_jeu(&jeu); // La main de chaque joueur contient 0 cartes car le niveau est 0 par défaut 

    printf("\nMéthode << demarrer_manche >>\n");
    demarrer_manche(&jeu, pile2, &pile_indice);

    printf("\nMéthode << ordonner_cartes >>\n");
    printf("Cartes non ordonnées : ");
    afficher_cartes(jeu.cartes, 4);
    ordonner_cartes(jeu.cartes, 4);
    printf("Cartes ordonnées : ");
    afficher_cartes(jeu.cartes, 4);

    printf("\nMéthode << verifier_manche >>\n");
    int indice_pile = 0;
    int ok1 = verifier_manche(&jeu, 3, &indice_pile); // Carte correcte
    int ok2 = verifier_manche(&jeu, 7, &indice_pile); // Carte incorrecte

    if(ok1){
        printf("La carte est correcte :) {résultat attendu}\n");
    }else{
        printf("La cartes est incorrecte {résultat non attendu}\n");
    }
    if(ok2){
        printf("La carte est correcte :) {résultat non attendu}\n");
    }else{
        printf("La cartes est incorrecte {résultat attendu}\n");
    }







    return 0;
}