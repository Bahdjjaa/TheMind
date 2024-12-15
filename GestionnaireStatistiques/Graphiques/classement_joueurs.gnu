set terminal png size 800,600
set output 'classement.png'
set title "Classement des joueurs"
set xlabel "Rang"
set ylabel "Joueur"
set style data histograms
set style fill solid
plot "classement_joueurs.dat" using 1:xtic(2) title "Classement"