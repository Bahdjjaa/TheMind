#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "GestionnaireServeur/Serveur.h"
#include "GestionnaireClient/GestionnaireClient.h"

#define ADRESSE_IP "127.0.0.1"
#define PORT 8080

void *thread_serveur(void *arg){
    int serveur_socket = initialiser_serveur(ADRESSE_IP, PORT);
    if(serveur_socket == -1){
        perror("Erreur lors de l'initialisation du serveur");
        retrun NULL;
    }

    boucle_principale(serveur_socket);
    retrun NULL;
}

/* void *thread_client(void *arg) 
Il faut ajouter une méthode boucle principale 
dans Gestionnaire client */

int main(){

    // Création des threads pour le serveur et les clients
    pthread_t server_thread;
    /*pthread_t client_threads[MAX_CLIENTS]*/

    // Lancer le serveur
    if(pthread_create(&server_thread, NULL, thread_serveur, NULL) != 0){
        perror("Erreur lors de la création du thread serveur");
        return EXIT_FAILURE; 
    }

    // Lancer les clients
    /*for(int i = 0; i < MAX_CLIENTS; i++){
        if(pthread_create(&client_threads[i], NULL, thread_client, NULL) != 0){
            perror("Erreur lors de la création du thread client");
            return EXIT_FAILURE;
        }
    }*/

   //Attendre la fin des threads
   pthread_join(server_thread, NULL);
   /*for(int i = 0; i < MAX_CLIENTS; i++){
    pthread_join(cleint_threads[i], NULL);
   } */

  return 0;
}