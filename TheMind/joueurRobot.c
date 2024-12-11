#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>


#define FIFO_PATH "game_fifo" // Chemin du tube nommé pour la communication

typedef struct {
    char nom[50]; 
    int carte;
} JoueurAction;

int main() {
    srand(time(NULL)); //initialise le générateur de nombre aleatoire
    JoueurAction action;
    strcpy(action.nom, "Robot"); 
    action.carte = rand() % 100 + 1; //carte aléatoire

    printf("%s joue la carte %d\n", action.nom, action.carte);

    //ouvre le FIFO en mode écriture
    int fd = open(FIFO_PATH, O_WRONLY);
    if(fd < 0) {
        perror("Erreur ouverture FIFO");
        return 1;
    }

    //envoie l'action (nom +carte) au gestionnaire
    write(fd, &action, sizeof(JoueurAction));
    close(fd); //fermeture du fifo apres envoi
    return 0;
}
