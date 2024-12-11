#include "GestionnaireClient.h"
#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
s

// Définir la structure du joueur avec le champ vie
typedef struct Joueur {
    int id;
    char nom[50];
} Joueur;


//Initialisation du socket et connexion au serveur
int initialiser_socket(const char *adress_ip, int port) {
    int sockfd = socket (AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("Erreur lors de la création du socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(adress_ip);


    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){
        perror("Erreur de connexion au serveur");
        close(sockfd);
        return -1;
    }
    printf("Connecté au serveur %s:%d\n", adress_ip, port);
    return sockfd;
} 

//Définir le nom du joueur 
void definir_nom_joueur(Joueur *joueur, int joueur_id, const char *nom){
    joueur->id= joueur_id;
    strncpy(joueur->nom, nom, 50);
    joueur->nom[49]= '\0';
    printf("Nom du joueur %d défini : %s\n");
}


// Fonction pour envoyer l'action du joueur (carte jouée)
void envoyer_action(int sockfd, const char *nom_joueur, int carte_jouee) {
    char action[100];
    snprintf(action, sizeof(action), "Joueur %s a joué la carte %d", nom_joueur, carte_jouee);
    if (send(sockfd, action, strlen(action), 0) < 0) {
        perror("Erreur lors de l'envoi de l'action");
    }
}

 //Fonction principale du  client
 void boucle_principale_client(int sockfd , const char *nom_joueur)
 {
    char buffer[BUFSIZ];
    int cartes_recues[BUFSIZ];

    while (1)
    {
        // Recevoir un message du serveur
        memset(buffer, 0, sizeof(buffer));
        if (recv(sockfd, buffer, sizeof(buffer), 0) >0)
        {
            printf("Serveur: %s\n", buffer);

            //si le serveur demande de jouer une carte
            if (strstr(buffer, "Cartes distribuées") != NULL)
            {
                //Recevoir les cartes du serveur
                int nb_Cartes; 
                recv(sockfd, &nb_Cartes, sizeof(int), 0);
                recv(sockfd, cartes_recues, nb_Cartes * sizeof(int), 0);

                printf("Vos cartes: ");
                for (int i = 0; i < nb_Cartes; i++)
                {
                    printf("%d", cartes_recues[i]);
                }
                printf("\n");

                //Jouer une carte (par exemple, choisir la première carte par défaut)
                int carte_jouee = cartes_recues[0]; //on demande au joueur quel nombre il veut mettre selon ce qu'il y a dans sa main
                printf("Vous jouez la carte : %d\n", carte_jouee);

                //Envoi du nom du joueur et de la carte jouée
                envoyer_action(sockfd, nom_joueur, carte_jouee);

                //Envoyer la carte jouée au serveur
                if (send(sockfd, &carte_jouee, sizeof(int), 0) < 0)
                {
                    perror("erreur lors de l'envoi de la carte");
                    break;
                }
            }
        }
        else if (strstr(buffer, "Partie terminée") !=NULL)
        {
            // Si le serveur indique la fin de la partie
            printf("Fin de la partie.\n");
            break;
        }
    }

     
    

    close(sockfd); //Fermer la connexion avec le serveur
 }

 int main(int argc, char *argv[])
 {
    if (argc !=4)
    {
        fprintf(stderr, "Usage: %s <adresse_ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *adress_ip = argv[1];
    int port = atoi(argv[2]);
     const char *nom_joueur = argv[3]; //on recupere le nom du joueur

    // Se connecter au serveur
    int client_socket = initialiser_socket(adress_ip, port);
    if(client_socket == -1)
    {
        return EXIT_FAILURE; 
    }


    //lancer la boucle principale du client
    boucle_principale_client(client_socket,nom_joueur);
    return EXIT_SUCCESS;

 }  







