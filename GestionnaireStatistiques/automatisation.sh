#!/usr/bin/bash

# Extraire les données
echo "extraction des données ..."
bash extraction.sh

# Gérer les graphiques
echo "Génération de graphiques ..."
cd Graphiques || { echo "Le répertoire Graphiques n'exite pas !"; return; }
gnuplot temps_reaction.gnu
gnuplot manches_reussies.gnu
gnuplot valeurs_echec.gnu
cd ..

# Générer le PDF
echo "Création du PDF ..."
cd StatistiquesFinales || { echo "Le répertoire StatistiquesFinales n'exite pas !"; return; }
pdflatex stats.tex
cd ..

echo "Statistiques générées avec succès !"

