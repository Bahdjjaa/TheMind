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

    printf("Serveur lancé sur le port %d",PORT);

    int socket_serveur = initialiser_serveur(adresse_ip, PORT);
    if(socket_serveur == -1){
        perror("Erreur lors de l'initialisation du serveur");
        return EXIT_FAILURE;
    }

    boucle_principale(socket_serveur);

    printf("AM OUT OF HERE\n");
    return 0;
}