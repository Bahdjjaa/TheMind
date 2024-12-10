#include <stdio.h>
#include <stdlib.h>
#include "Serveur.h"

#define ADRESSE_IP "127.0.0.1" // Adresse IP locale
#define PORT 8080 // Port du serveur

int main(){
    // Initialiser le serveur
    int serveur_socket = initialiser_serveur(ADRESSE_IP, PORT);
    if(serveur_socket == -1){
        printf("Erreur lors de l'initialisation du serveur");
        return EXIT_FAILURE;
    }

    printf("Serveur initialisé sur %s:%d\n", ADRESSE_IP, PORT);

    // Lancer la boucle principale
    boucle_principale(serveur_socket);

}