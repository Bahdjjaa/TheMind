#include <stdio.h>
#include <stdlib.h>
#include "GestionnaireClient.h"

#define ADRESSE_IP "127.0.0.1"
#define PORT 8080

int main(){
    // Initialiser le socket client
    int client_socket = initialiser_socket(ADRESSE_IP, PORT);
    if (client_socket == -1) {
        perror("Erreur lors de la connexion au serveur");
        return EXIT_FAILURE;
    }

    printf("Client connecté au serveur %s:%d\n", ADRESSE_IP, PORT);

    // Envoie d'une action de test
    envoyer_action(client_socket, "TestClient", 3);
    

    return 0;
}