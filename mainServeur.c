#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "GestionnaireServeur/Serveur.h"

#define PORT 8080

int main(int argc, char *argv[]){

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <adresse_ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *adresse_ip = argv[1];
    int nb_joueurs;

    printf("Serveur lancé sur le port %d\n",PORT);

    int socket_serveur = initialiser_serveur(adresse_ip, PORT);
    if(socket_serveur == -1){
        perror("Erreur lors de l'initialisation du serveur");
        return EXIT_FAILURE;
    }

    printf("Entrez le nombre de joueurs : ");
    while(scanf("%d", &nb_joueurs) != 1 || nb_joueurs <= 1){
        printf("Saisie invalide, Le nombre de joueurs doit être entre 2 et 4 (inclus)");

        // Videz le buffer en cas de saisie invalide
        while(getchar() != '\n');
    }


    boucle_principale(socket_serveur, nb_joueurs);

    printf("AM OUT OF HERE\n");
    return 0;
}