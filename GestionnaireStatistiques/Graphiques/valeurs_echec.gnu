if(system("test -s valeurs_echec.dat") == 0){
    set terminal png size 800,600
    set output 'valeurs_echec.png'
    set title "Histogramme des valeurs ayant causé l'échec"
    set xlabel "Valeur"
    set ylabel "Fréquence"
    set style data histograms
    set style fill solid
    plot "valeurs_echec.dat" using 2:1 title "Fréquence"
} else {
    set terminal png size 800,600
    set output "valeurs_echec.png"
    set title "Aucune donnée disponible"
    set xlabel ""
    set ylabel ""
    set xtics format ""
    set ytics format ""
    unset border
    plot NaN notitle
}