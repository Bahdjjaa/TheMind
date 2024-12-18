#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "GestionnaireClient/ClientHumain.h"

#define PORT 8080

int main(int argc, char *argv[]){
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <adresse_ip> <nom>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *adresse_ip = argv[1]; // Récupérer l'adresse IP passée en paramètre
    char *nom = argv[2]; // Récupérer le nom passé en paramètre



    printf("Connexion au serveur %s:%d...\n", adresse_ip, PORT);

    int client_socket = initialiser_socket(adresse_ip, PORT);
    if(client_socket== -1){
        perror("Erreur lors de l'initialisation du serveur");
        return EXIT_FAILURE;
    }

    boucle_principale_client_humain(client_socket, nom);
    return 0;
}