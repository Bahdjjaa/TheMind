#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "GestionnaireClient/ClientHumain.h"

#define ADRESSE_IP "127.0.0.1"
#define PORT 8080

int main(int argc, char *argv[]){
    /*
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    */

   /*Récupérer le nom du client*/
   char *nom = "Joueur";

    printf("Connexion au serveur %s:%d...\n", ADRESSE_IP, PORT);

    int client_socket = initialiser_socket(ADRESSE_IP, PORT);
    if(client_socket== -1){
        perror("Erreur lors de l'initialisation du serveur");
        return EXIT_FAILURE;
    }

    boucle_principale_client_humain(client_socket, nom);

    printf("AM OUT OF HERE\n");
    return 0;
}