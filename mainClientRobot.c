#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "GestionnaireClient/ClientRobot.h" 

#define PORT 8080

int main(int argc, char *argv[]){
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <adresse_ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *adresse_ip = argv[1]; // Récupérer l'adresse IP passée en paramètre

   /*Nom du robot*/
   char *nom = "ChatGPT";

    printf("Connexion au serveur %s:%d...\n", adresse_ip, PORT);

    int robot_socket = initialiser_socket(adresse_ip, PORT);
    if(robot_socket== -1){
        perror("Erreur lors de l'initialisation du serveur");
        return EXIT_FAILURE;
    }

    boucle_principale_client_robot(robot_socket, nom);
    return 0;
}