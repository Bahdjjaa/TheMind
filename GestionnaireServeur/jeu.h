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
    int socket;
    char nom[TAILLE_NOM_JOUEUR];
    Carte *main; // Pointeur vers les cartes en main
    int nb_cartes; // Nombre de cartes en main
} Joueur;

// Structures états du jeu
typedef struct
{
    int niveau; //Niveau actuel
    int vies; // Nombre de vies restantes
    Joueur *joueurs; // Liste des joeurs
    int nb_joueurs;  // Nombre total de joueurs
    Carte *cartes; // Pile des cartes distribuées
    int nb_cartes; // Nombres de cartes distribuées
} Etats_Jeu;


/* ===== Partie collection de données ==== */
typedef struct
{
    Joueur joueur;  // Information du joueur
    int manches_reussies; // Nombre de manches réussies dans la partie
    int nb_vals_echec; // Le nombre de valeurs que le joueurs a mal joué
    int nb_vals_correctes; // Le nombre de valeurs que le joueurs a joué correctement
    double temps_reaction; //Temp de réaction moyen (en secondes)
} StatJoueur; 

typedef struct 
{
    int total_manches; // Total des manches jouées
    StatJoueur* stat_joueurs; // Tables des stats des joueurs
    int* vals_echec;
    int nb_joueurs; // Nombre total de joueurs
}StatPartie;

/* ===== Fin Partie collection de données ==== */

// Méthode pour générer les carte aléatoirement
void generer_cartes(Carte* pile);

// Méthode pour mélanger les cartes
void melanger_cartes(Carte* pile, int nb_cartes);

// Méthode pour initialiser un joueur
void initialiser_joueur(Joueur *joueur, const char *nom);

// Méthode pour initialiser l'état du jeu
void initialiser_jeu(Etats_Jeu *etat, int nb_joueurs);

// Méthode pour initialiser les statiqtiques de la partie
void initialiser_stats_partie(Etats_Jeu *jeu,StatPartie *stats);

// Méthode pour mettre à jour les statistiques
void mettre_a_jour_statistiques(StatPartie *stats, int id_joueur, int manche_reussie, int valeur_echec, double temps_reaction);

// Méthode pour sauvgarder les statistiques dans un fichier text
void sauvegarder_statistiques(StatPartie *stats);

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