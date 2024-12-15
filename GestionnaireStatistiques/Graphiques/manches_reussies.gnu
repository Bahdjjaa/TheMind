set terminal png size 800,600
set output 'manches_reussies_par_joueur.png'
set title "Manches réussies par joueur"
set xlabel "Joueur"
set ylabel "Nombre de manches"
set style data histograms
set style fill solid
plot "manches_reussies_par_joueur.dat" using 2:xtic(1) title "Manches réussies"