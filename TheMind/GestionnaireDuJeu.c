#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>


#define FIFO_PATH "jeu_fifo" //chemin du tube nommé pour la communication
#define FIFO_FROM_MANAGER "jeu_fifo_in" 



#define Max_Joueurs 4 //NB max de joueurs
#define Max_Cartes 100

//Structure représentant une action d'un joueur
typedef struct {
    char nom[50]  ;//id du joueur
    int carte; //Carte jouée par le joueur
}JoueurAction;


void envoyerMessageTous(const char *message){
    int fd = open(FIFO_FROM_MANAGER, O_WRONLY);
    if (fd < 0) {
        perror("Erreur ouverture FIFO pour envoyer message");
        return 1;
    }
    write(fd, message,strlen(message));
    close(fd);
}

int verifierOrdre(int derniereCarte, int nouvelleCarte){
    return nouvelleCarte > derniereCarte;
}




//fonction pour distribuer les cartes aux joueurs 
void distribuer_cartes(int joueurs[], int nb_joueurs, int manche) {
    for (int i= 0; i < nb_joueurs; i++) {
        //Genere une carte aléatoire entre 1 et MAX_CARTES
        joueurs[i] = rand() %Max_Cartes + 1 ; 
        printf("Joueur %d recoit la carte %d\n , i+1, Joueur[i]");
    }
}

//FOnction pour verfier si les cartes jouées sont dans l'ordre croissant
int verifier_Ordre(int cartes [], int nb_joueurs){
    for (int i=1; i < nb_joueurs; i++) {
        if (cartes[i -1] > cartes[i]){
            return 0; //Retourne 0 si une erreur dans l'ordre est detéctée
        }
    }
    return 1;
}

int main(){
    //creation d'un tube nommé (fifo) pour la communication
    mkfifo(FIFO_PATH, 0666);
    mkfifo(FIFO_FROM_MANAGER, 0666);

    int nb_joueurs=3;
    int cartes[Max_Joueurs];
    JoueurAction action; //pour representer une action recue

    int manche = 1; //pour compter les manches
    int cartes_jouees[Max_Joueurs] = {0}; // Déclarez ici
    while(1) {
        printf("=== début de la manche %d ===\n",manche);

        //Ouvre le FIFO en mode lecture pour recevoir les actions des joueurs
        int fd = open(FIFO_PATH,O_RDONLY);
        int cartes_jouees[Max_Joueurs] = {0}; //tab pour enregistrer les cartes jouées
        int nb_cartes_jouees = 0 ;
        
        //ON attend que tous les joueurs jouent leurs cartes 
        while (nb_cartes_jouees < nb_joueurs) {
            //lit de l'action depuis le FIFO
            read(fd, &action,sizeof(JoueurAction));
            printf("Joueur %d joue la carte %d\n, action.id, action.carte");
            cartes_jouees[nb_cartes_jouees++] = action.carte; //enregistration de la carte jouée
        };
        close(fd);
    }

    if (verifier_Ordre(cartes_jouees,nb_joueurs)) {
        printf("Manche %d réussie !\n", manche);
        manche++; //on passe a la manche suivante
    } else {
        printf("manche perdue ! Recommencez.\n");
        manche = 1; //Retour à la premiere manche en cas d'erreur
    }

    memset(cartes_jouees, 0, sizeof(cartes_jouees));
   //supprime le FIFO à la fin du programme
   unlink(FIFO_PATH);
   return 0;
}
