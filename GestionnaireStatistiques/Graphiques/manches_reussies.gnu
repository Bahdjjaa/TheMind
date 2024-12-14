if(system("test -s manches_reussies_par_joueur.dat") == 0){
    set terminal png size 800,600
    set output 'manches_reussies_par_joueur.png'
    set title "Manches réussies par joueur"
    set xlabel "Joueur"
    set ylabel "Nombre de manches"
    set style data histograms
    set style fill solid
    plot "manches_reussies_par_joueur.dat" using 2:xtic(1) title "Manches réussies"
}else{
    set terminal png size 800,600
    set output "manches_reussies_par_joueur.png"
    set title "Aucune donnée disponible"
    set xlabel ""
    set ylabel ""
    set xtics format ""
    set ytics format ""
    unset border
    plot NaN notitle
}

