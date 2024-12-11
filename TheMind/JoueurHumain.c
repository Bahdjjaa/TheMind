#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>


#define FIFO_PATH "jeu_fifo" //chemin du tube nommé pour la communication
#define FIFO_FROM_MANAGER "jeu_fifo_in" 

typedef struct {
    char nom[50];
    int carte; 
} JoueurAction;


void afficherCartes(int *cartes, int nbCartes){
    printf("\nVos cartes:");
    for (int i = 0; i< nbCartes; i++){
        printf("%d", cartes[i]);
    }
    printf("\n");
}

void recevoirMiseAjour(int fd_FROM_MANAGER){
 char buffer[256];
 ssize_t n;

 //lis les messages du gestionnaire et affiche les
 while((n= read(fd_FROM_MANAGER, buffer, sizeof(buffer) - 1))>0){
    buffer[n] = '\0';
    printf("%S\n", buffer);
}
}

int main(int argc, char *argv[]){
//verifier que le programme a recu le bon nombre d'argument
    if (argc != 3) {
     printf("Usage :%s <id_joueur> <carte>\n" , argv[0]);
     return 1;
    }

    char nom[50];
    strncpy(nom, argv[1], sizeof(nom) - 1);
    nom[sizeof(nom) - 1] = '\0';

    //conenxion au fifo du gestionnaire
    int fd_FIFO_PATH = open(FIFO_PATH, O_WRONLY);
    if(fd_FIFO_PATH < 0){
    perror("erreur d'ouverture fifo vers le gestionnaire");
    return 1;
    }

    int fd_FROM_MANAGER = open(FIFO_FROM_MANAGER, O_RDONLY);
    if(fd_FROM_MANAGER < 0) {
        perror("erreur d'ouverture depuis le gestionnaire");
        close(fd_FIFO_PATH);
        return 1;
    }


    printf("Bienvenue, %s ! Attente du début de la partie \n", nom);

    //boucle principale
    while(1) {
    int nbCartes;
    int cartes[10];
    JoueurAction action;


    //recepetion de la mise à jour
    recevoirMiseAjour(fd_FROM_MANAGER);
    printf("Combien de cartes avez-vous cette manche?");
    scanf("%d", &nbCartes);

    if(nbCartes <= 0 || nbCartes >10){
        printf("nombre de cartes invalide.\n");
        continue;
    }

    printf("entrez vos cartes (une par une) : \n");
    for (int i=0; i<nbCartes; i++) {
        scanf("%d",&cartes[i]);
    }

    afficherCartes(cartes,nbCartes);
    for (int i=0 ; i< nbCartes; i++){
        printf("appuyez Entrée pour jouer la carte %d..\n",cartes[i]);
        getchar(); //attente d'une touche pour jouer
        strncpy(action.nom, nom, sizeof(action.nom) - 1);
        action.nom[sizeof(action.nom) - 1] = '\0';
        action.carte = cartes[i];

        //envoi au gestionnaire
        write(fd_FIFO_PATH, &action, sizeof(JoueurAction));
        printf("carte %d jouée ./n", cartes[i]);
    }

    //verifier la fin de la partie
    printf("Attente des autres joueurs...\n");
    recevoirMiseAjour(fd_FROM_MANAGER);

    }

    //fermeture des fifos
    close(fd_FIFO_PATH);
    close(fd_FROM_MANAGER);
    return 0;

}

