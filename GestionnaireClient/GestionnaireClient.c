#include "GestionnaireClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


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
    printf("nom du joueur %d défini : %s\n",joueur_id,joueur->nom);
}


// Fonction pour envoyer le nom du joueur
void envoyer_nom_joueur(int sockfd, const char *nom_joueur){
    if (send(sockfd, nom_joueur, strlen(nom_joueur) + 1, 0) < 0){
        perror("Erreur lors de l'envoi du nom");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Nom du joueur envoyé au serveur : %s\n", nom_joueur);
}


 //Fonction principale du  client
 void boucle_principale_client(int sockfd , const char *nom_joueur)
 {
    char buffer[BUFSIZ];
    int cartes_recues[BUFSIZ];


    //Definir le joueur (initialiser la structure)
    Joueur joueur;
    definir_nom_joueur(&joueur, 1, nom_joueur);

    //envoyer le nom du joueur au serveur
    envoyer_nom_joueur(sockfd, joueur.nom);

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

                //Demander au joueur de choisir un nombre parmi ses cartes
                int carte_jouee = -1;
                while(carte_jouee < 0 || carte_jouee >= nb_Cartes){
                    printf("Choisissez une carte à jouer (numero entre 0 et %d):", nb_Cartes - 1);
                    scanf("%d", &carte_jouee);

                    //verifier que le choix est dans les limites des indices des cartes
                    if (carte_jouee < 0 || carte_jouee >= nb_Cartes){
                        printf("Choix invalide, veuillez entrer un numéro valide de carte.\n"); 
                    }
                    else{
                        // Le joueur a choisi un index invalide, on peut lui indiquer quelle carte il joue
                        printf("vous jouez la carte : %d\n", cartes_recues[carte_jouee]);
                    }

                }

               

                //Envoyer la carte jouée au serveur
                if (send(sockfd, &cartes_recues[carte_jouee], sizeof(int), 0) < 0)
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