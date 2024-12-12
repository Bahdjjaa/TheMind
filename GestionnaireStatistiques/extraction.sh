#!/usr/bin/sh

# Script d'extraction des statistiques

# Temps de réaction par joueur
awk '/Joueur:/ { joueur=$2 }
    /Temps réaction moyenne:/ { print joueur, $4}' stats_patie.txt > temps_reaction_par_joueur.dat


# Nombre de manches réussies par joueur
awk '/Joueur:/ { joueur=$2 }
    /Manches réussies:/ {print joueur, $3}' stats_patie.txt > manches_reussies_par_joueur.dat


# Les valeurs qui ont causé l'échec
awk "/a causé l'échec/ { print $1, $6 }" stats_patie.txt > valeurs_echec.dat