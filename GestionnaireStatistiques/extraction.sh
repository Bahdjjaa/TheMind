#!/usr/bin/bash

# Script d'extraction des statistiques

# Temps de réaction par joueur
awk '/Joueur:/ { joueur=$2 }
    /Temps réaction moyenne:/ { print joueur, $4}' stats_partie.txt > Graphiques/temps_reaction_par_joueur.dat


# Nombre de manches réussies par joueur
awk '/Joueur:/ { joueur=$2 }
    /Manches réussies:/ {print joueur, $3}' stats_partie.txt > Graphiques/manches_reussies_par_joueur.dat


# Les valeurs qui ont causé l'échec
awk '/Numéro/ { print $2, $4 }' stats_partie.txt > Graphiques/valeurs_echec.dat

# Extraction du classement des joueurs
awk '/Rang/ { 
    rang = $2
    joueur = $4
    printf "Rang %d : %s\n", rang, joueur
}' stats_partie.txt > classement_joueurs.txt
