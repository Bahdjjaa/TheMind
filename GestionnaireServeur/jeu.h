#ifndef JEU_H
#define JEU_H

#define NB_CARTES_TOTAL 100
#define TAILLE_NOM_JOUEUR 50

// Structure représentant une carte
typedef struct
{
    int numero;
    int est_jouee; // 0 = non jouée, 1 = jouée
} Carte;

// Structure représentant un joueur
typedef struct
{
    int id;
    char nom[TAILLE_NOM_JOUEUR];
    Carte *main; // Pointeur vers les cartes en main
    int nb_cartes; // Nombre de cartes en main
} Joueur;

// Structures états du jeu
typedef struct
{
    int niveau; //Niveau actuel
    int vies; // Nombre de vies restantes
    int shurikens; // Nombre de shurikens disponibles
    Joueur *joueurs; // Liste des joeurs
    int nb_joueurs;  // Nombre total de joueurs
    Carte *cartes; // Pile des cartes distribuées
    int nb_cartes; // Nombres de cartes distribuées
} Etats_Jeu;

// Méthode pour générer les carte aléatoirement
void generer_cartes(Carte* pile);

// Méthode pour mélanger les cartes
void melanger_cartes(Carte* pile, int nb_cartes);

// Méthode pour initialiser un joueur
void initialiser_joueur(Joueur *joueur, int id, const char *nom);

// Méthode pour initialiser l'état du jeu
void initialiser_jeu(Etats_Jeu *etat, int nb_joueurs, int vies, int shurikens);

// Méthode pour distribuer les cartes aux joueurs
void distribuer_cartes(Etats_Jeu *etat,Carte* pile, int *pile_indice);

// Fonction utilitaire pour ordonner les cartes
void ordonner_cartes(Carte *pile, int nb_cartes);

// Méthode pour démarer une nouvelle manche
void demarrer_manche(Etats_Jeu *etat, Carte* pile, int *pile_indice);

// Méthode pour finir une manche (réussie ou échouée)
int verifier_manche(const Etats_Jeu *jeu, int carte_jouee, int *indice_pile);

// Méthode pour afficher les cartes
void afficher_cartes(const Carte *cartes, int nb_cartes);

// Méthode pour afficher l'état du jeu
void afficher_etat_jeu(const Etats_Jeu *jeu);

#endif