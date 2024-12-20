#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

// Générer les cartes
void generer_cartes(Carte *pile)
{
    for (int i = 0; i < NB_CARTES_TOTAL; i++)
    {
        pile[i].numero = i + 1;
        pile[i].est_jouee = 0; // Non utilisé dans le serveur
    }
}

// Mélanger les cartes
void melanger_cartes(Carte *pile, int nb_cartes)
{
    srand(time(NULL));
    for (int i = nb_cartes - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        Carte temp = pile[i];
        pile[i] = pile[j];
        pile[j] = temp;
    }
}

// Joueur par défaut
void initialiser_joueur(Joueur *joueur, const char *nom)
{
    joueur->main = NULL;
    strncpy(joueur->nom, nom, sizeof(joueur->nom) - 1);
    joueur->nom[sizeof(joueur->nom) - 1] = '\0';
    joueur->nb_cartes = 0;
}

// Etat du jeu par défaut
void initialiser_jeu(Etats_Jeu *jeu, int nb_joueurs)
{
    jeu->niveau = 1;
    jeu->vies = nb_joueurs;
    jeu->nb_joueurs = nb_joueurs;
    jeu->joueurs = malloc(nb_joueurs * sizeof(Joueur));
    for (int i = 0; i < nb_joueurs; i++)
    {
        Joueur joueur = jeu->joueurs[i];
        char *nom = jeu->joueurs[i].nom;
        initialiser_joueur(&joueur, nom);
    }
}

// Initialiser les statistiques de la partie
void initialiser_stats_partie(Etats_Jeu *jeu, StatPartie *stats)
{
    // stats->id_partie = id;
    stats->total_manches = 0;
    stats->nb_joueurs = jeu->nb_joueurs;
    stats->vals_echec = malloc(NB_CARTES_TOTAL * sizeof(int));
    stats->stat_joueurs = malloc(stats->nb_joueurs * sizeof(StatJoueur));

    if (stats->vals_echec == NULL)
    {
        perror("Erreur d'allocation mémoire pour vals_echec");
        exit(EXIT_FAILURE); // Ou gérer autrement l'erreur
    }

    // Initialiser les statistiques des joueurs
    for (int i = 0; i < jeu->nb_joueurs; i++)
    {
        stats->stat_joueurs[i].joueur = jeu->joueurs[i];
        stats->stat_joueurs[i].manches_reussies = 0;
        stats->stat_joueurs[i].nb_vals_echec = 0;
        stats->stat_joueurs[i].nb_vals_correctes = 0;
        stats->stat_joueurs[i].temps_reaction = 0.0;
    }
}

// Mettre à jour les statistiques
void mettre_a_jour_statistiques(StatPartie *stats, int id_joueur, int manche_reussie, int valeur_echec, double temps_reaction)
{
    if (manche_reussie == 1) /*Fin de la manche, toutes mes cartes jouées sont correctes*/
    {
        for (int i = 0; i < stats->nb_joueurs; i++)
        {
            stats->stat_joueurs[i].manches_reussies++;
        }
        stats->total_manches++;
    }
    else if (manche_reussie == 2)
    {                                                                    /*La manche n'est pas encore terminée mais la valeur envoyée est correcte*/
        stats->stat_joueurs[id_joueur].nb_vals_correctes++;              // Incrémenter le nombre de valeurs correctes jouées
        stats->stat_joueurs[id_joueur].temps_reaction += temps_reaction; // Pour calculer la moyenne
    }
    else /*La carte jouée a causé l'échec*/
    {
        stats->stat_joueurs[id_joueur].nb_vals_echec++;
        stats->vals_echec[valeur_echec - 1]++;                           // Incrémenter le nombre de fois que la valeur à causer l'échec
        stats->stat_joueurs[id_joueur].temps_reaction += temps_reaction; // Pour calculer la moyenne
        stats->total_manches++;
    }
}

// Sauvgarder les statisqtiques dans un fochiers
void sauvegarder_statistiques(StatPartie *stats)
{
    char chemin_fichier[256];
    snprintf(chemin_fichier, sizeof(chemin_fichier), "GestionnaireStatistiques/stats_partie.txt");

    printf("Chemin du fichier : %s\n", chemin_fichier);

    FILE *fd = fopen(chemin_fichier, "w");
    if (fd == NULL)
    {
        perror("Erreur lors de l'ouverture du fichier de statistiques");
        return;
    }
    fprintf(fd, "=== STATISTIQUES DE LA PARTIE ===\n");
    fprintf(fd, "Total Manches: %d\n", stats->total_manches);
    fprintf(fd, "Nombre de joueurs: %d\n", stats->nb_joueurs);

    for (int i = 0; i < stats->nb_joueurs; i++)
    {
        StatJoueur *s = &stats->stat_joueurs[i];
        fprintf(fd, "Joueur: %s \n", s->joueur.nom);
        fprintf(fd, "Manches réussies: %d \n", s->manches_reussies);
        fprintf(fd, "Le nombre de fois qu'il a joué une carte correcte: %d \n", s->nb_vals_correctes);
        fprintf(fd, "Le nombre de fois qu'il a causé l'échec échec: %d \n", s->nb_vals_echec);
        fprintf(fd, "Temps réaction moyenne: %.2f \n", s->temps_reaction / stats->total_manches);
    }

    // Classement basé sur la différence entre valeurs correctes et incorrectes
    fprintf(fd, "Classement des joueurs\n");
    for (int i = 0; i < stats->nb_joueurs - 1; i++)
    {
        for (int j = 0; j < stats->nb_joueurs - i - 1; j++)
        {
            // Calculer les différences pour les joueurs
            int diff_j = stats->stat_joueurs[j].nb_vals_correctes - stats->stat_joueurs[j].nb_vals_echec;
            int diff_j1 = stats->stat_joueurs[j + 1].nb_vals_correctes - stats->stat_joueurs[j + 1].nb_vals_echec;

            // permuter les joueurs
            if (diff_j < diff_j1)
            {
                StatJoueur temp = stats->stat_joueurs[j];
                stats->stat_joueurs[j] = stats->stat_joueurs[j + 1];
                stats->stat_joueurs[j + 1] = temp;
            }
        }
    }

    // Afficher le classement
    for (int i = 0; i < stats->nb_joueurs; i++)
    {
        StatJoueur *s = &stats->stat_joueurs[i];
        fprintf(fd, "Rang %d : %s\n", i + 1, s->joueur.nom);
    }

    fprintf(fd, "Le nombre de fois que chaque valeur a causé l'échec: \n");
    for (int i = 0; i < NB_CARTES_TOTAL; i++)
    {
        fprintf(fd, "Numéro %d , %d fois \n", (i + 1), stats->vals_echec[i]);
    }

    fprintf(fd, "==================================\n");
    fclose(fd);

    /**** Partie génération du PDF des statistiques****/
    // Donner les permission d'exécution au script automatisation.sh
    system("chmod +x GestionnaireStatistiques/automatisation.sh");

    // Lancer le script automatisation.sh
    system("cd GestionnaireStatistiques && ./automatisation.sh");

    //Attendre un petit délai
    sleep(1);

    // Suppression des fichiers .dat et .png et .txt après utilisation
    system("rm GestionnaireStatistiques/Graphiques/*dat GestionnaireStatistiques/Graphiques/*png GestionnaireStatistiques/*.txt");
}

void distribuer_cartes(Etats_Jeu *jeu, Carte *pile, int *pile_indice)
{
    // Allouer suffisamment d'espace pour toutes les cartes distribuées
    jeu->nb_cartes = jeu->nb_joueurs * jeu->niveau;
    jeu->cartes = malloc(jeu->nb_cartes * sizeof(Carte));

    // Pour savoir combien de cartes ont été utilisées
    int cartes_utilisees = 0;

    for (int i = 0; i < jeu->nb_joueurs; i++)
    {
        jeu->joueurs[i].nb_cartes = jeu->niveau;                    // Chaque joueur reçoit 'niveau' cartes
        jeu->joueurs[i].main = malloc(jeu->niveau * sizeof(Carte)); // Allocation mémoire pour les cartes

        if (jeu->joueurs[i].main == NULL)
        {
            perror("Erreur lors de l'allocation mémoire pour la main du joueur");
            exit(EXIT_FAILURE);
        }

        // Donner les cartes aux joueurs
        for (int j = 0; j < jeu->niveau; j++)
        {
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

void ordonner_cartes(Carte *pile, int nb_cartes)
{
    for (int i = 0; i < nb_cartes - 1; i++)
    {
        for (int j = 0; j < nb_cartes - i - 1; j++)
        {
            if (pile[j].numero > pile[j + 1].numero)
            {
                // Échanger les cartes
                Carte carte_temp = pile[j];
                pile[j] = pile[j + 1];
                pile[j + 1] = carte_temp;
            }
        }
    }
}

void afficher_cartes(const Carte *cartes, int nb_cartes)
{
    if (cartes != NULL)
    {
        for (int i = 0; i < nb_cartes; i++)
        {
            printf("%d ", cartes[i].numero);
        }
        printf("\n");
    }
    else
    {
        printf("Pile vide \n");
    }
}

void afficher_etat_jeu(const Etats_Jeu *jeu)
{
    printf("État du jeu :\n");
    printf("Niveau : %d\n", jeu->niveau);
    printf("Vies restantes : %d\n", jeu->vies);
    printf("Nombre de joueurs : %d\n", jeu->nb_joueurs);

    for (int i = 0; i < jeu->nb_joueurs; i++)
    {
        printf("%s : ", jeu->joueurs[i].nom);
        afficher_cartes(jeu->joueurs[i].main, jeu->joueurs[i].nb_cartes);
    }
}

void demarrer_manche(Etats_Jeu *jeu, Carte *pile, int *pile_indice)
{
    distribuer_cartes(jeu, pile, pile_indice); // Distribuer les cartes au niveau du serveur

    printf("Manche %d commencée\n", jeu->niveau);

    for (int i = 0; i < jeu->nb_joueurs; i++)
    {
        printf("Joueur : %s ", jeu->joueurs[i].nom);
        afficher_cartes(jeu->joueurs[i].main, jeu->joueurs[i].nb_cartes);
    }
}

int verifier_manche(const Etats_Jeu *jeu, int carte_jouee, int *indice_pile)
{
    if (carte_jouee == jeu->cartes[*indice_pile].numero)
    {
        // La carte est correcte, on passe à la suivante
        return 1;
    }
    else
    {
        // la carte est incorrecte
        return 0;
    }
}
