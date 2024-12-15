set terminal png size 800,600
set output 'temps_reaction_par_joueur.png'
set title "Temps moyen de réaction par joueur"
set xlabel "Joueur"
set ylabel "Temps moyen (s)"
set style data histograms
set style fill solid
plot "temps_reaction_par_joueur.dat" using 2:xtic(1) title "Temps moyen"
