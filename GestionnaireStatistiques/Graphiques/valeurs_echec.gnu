set terminal png size 800,600
set output 'valeurs_echec.png'
set title "Histogramme des valeurs ayant causé l'échec"
set xlabel "Valeur"
set ylabel "Fréquence"
set style data histograms
set style fill solid
plot "valeurs_echec.dat" using 2:xtic(1) title "Fréquence"