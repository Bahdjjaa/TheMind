#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Générer les cartes aléatoirement
void generer_cartes(Carte* pile){
    for(int i = 0; i < NB_CARTES_TOTAL; i++){
        pile[i].numero = i+1;
        pile[i].est_jouee = 0;
    }
}

// Mélanger les cartes
void melanger_cartes(Carte* pile, int nb_cartes){
    srand(time(NULL));
    for(int i = nb_cartes - 1; i > 0; i--){
        int j = rand() % (i+1);
        Carte temp = pile[i];
        pile[i] = pile[j];
        pile[j] = temp;
    }
}

// Joueur par défaut 
void initialiser_joueur(Joueur *joueur, int id, const char *nom){
    joueur->id = id;
    joueur->main = NULL;
    strncpy(joueur->nom, nom, sizeof(joueur->nom) - 1);
    joueur->nom[sizeof(joueur->nom) - 1] = '\0';
    joueur->nb_cartes = 0;
}

// Etat du jeu par défaut
void initialiser_jeu(Etats_Jeu *jeu, int nb_joueurs, int vies, int shurikens){
    jeu->niveau = 0;
    jeu->vies = vies;
    jeu->shurikens = shurikens;
    jeu->nb_joueurs = nb_joueurs;
    jeu->joueurs = malloc(nb_joueurs * sizeof(Joueur));
    for(int i = 0; i < nb_joueurs; i++){
        Joueur joueur = jeu->joueurs[i];
        char *nom = jeu->joueurs[i].nom;
        initialiser_joueur(&joueur, i+1, nom);
    }
}


void distribuer_cartes(Etats_Jeu *jeu, Carte* pile, int *pile_indice){

    // Allouer suffisamment d'espace pour toutes les cartes distribuées
    jeu->nb_cartes = jeu->nb_joueurs * jeu->niveau;
    jeu->cartes = malloc(jeu->nb_cartes * sizeof(Carte));

    // Pour savoir combien de cartes ont été utilisées
    int cartes_utilisees = 0;

    for(int i = 0; i < jeu->nb_joueurs; i++){
        jeu->joueurs[i].nb_cartes = jeu->niveau; // Chaque joueur reçoit 'niveau' cartes
        jeu->joueurs[i].main = malloc(jeu->niveau * sizeof(Carte)); // Allocation mémoire pour les cartes

        if(jeu->joueurs[i].main == NULL){
            perror("Erreur lors de l'allocation mémoire pour la main du joueur");
            exit(EXIT_FAILURE);
        }

        // Donner les cartes aux joueurs
        for(int j = 0; j < jeu->niveau; j++){
            Carte carte_distribuee = pile[*pile_indice];
            jeu->joueurs[i].main[j] = carte_distribuee; 

            // Mettre à jour les cartes distribuées dans jeu->cartes
            jeu->cartes[cartes_utilisees] = carte_distribuee;
            cartes_utilisees++;
            (*pile_indice)++;
        }
    }

    // Ordonner les cartes distribuées pour les tester plus tard
    ordonner_cartes(jeu->cartes, jeu->nb_cartes); 
}

void ordonner_cartes(Carte *pile, int nb_cartes){
    for(int i = 0; i < nb_cartes - 1; i++){
        for(int j = 0; j < nb_cartes - i - 1; j++){
            if(pile[j].numero > pile[j+1].numero){
                // Échanger les cartes
                Carte carte_temp = pile[j];
                pile[j] = pile[j+1];
                pile[j + 1] = carte_temp;
            }
        }
    }
}

void afficher_cartes(const Carte *cartes, int nb_cartes){
    if(cartes != NULL){
        for(int i = 0; i < nb_cartes; i++){
            printf("%d ", cartes[i].numero);
        }
        printf("\n");
    }else{
        printf("Pile vide \n");
    }
   
}

void afficher_etat_jeu(const Etats_Jeu *jeu){
    printf("État du jeu :\n");
    printf("Niveau : %d\n", jeu->niveau);
    printf("Vies restantes : %d\n", jeu->vies);
    printf("Shurikens disponibles : %d\n", jeu->shurikens);
    printf("Nombre de joueurs : %d\n", jeu->nb_joueurs);

    for (int i = 0; i < jeu->nb_joueurs; i++) {
        printf("Joueur %d : ", jeu->joueurs[i].id);
        afficher_cartes(jeu->joueurs[i].main, jeu->joueurs[i].nb_cartes);
    }
}


void demarrer_manche(Etats_Jeu *jeu, Carte* pile, int *pile_indice){
    jeu->niveau++; // Passer au niveau suivant
    distribuer_cartes(jeu, pile, pile_indice); // Distribuer les cartes

    printf("Manche %d commencée\n", jeu->niveau);

    for(int i = 0; i < jeu->nb_joueurs; i++){
        printf("Joueur %d : ", jeu->joueurs[i].id);
            afficher_cartes(jeu->joueurs[i].main, jeu->joueurs[i].nb_cartes);
    }
}

int verifier_manche(const Etats_Jeu *jeu, int carte_jouee, int *indice_pile){
    if(carte_jouee == jeu->cartes[*indice_pile].numero){
        // La carte est correcte, on passe à la suivante
        (*indice_pile)++;
        return 1;
    }else{
        // la carte est incorrecte
        return 0;
    }
}

