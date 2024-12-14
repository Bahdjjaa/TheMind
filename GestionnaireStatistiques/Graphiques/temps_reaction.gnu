if(system("test -s temps_reaction_par_joueur.dat") == 0){
    set terminal png size 800,600
    set output 'temps_reaction_par_joueur.png'
    set title "Temps moyen de réaction par joueur"
    set xlabel "Joueur"
    set ylabel "Temps moyen (s)"
    set style data histograms
    set style fill solid
    plot "temps_reaction_par_joueur.dat" using 2:xtic(1) title "Temps moyen"
}else{
    set terminal png size 800,600
    set output "temps_reaction_par_joueur.png"
    set title "Aucune donnée disponible"
    set xlabel ""
    set ylabel ""
    set xtics format ""
    set ytics format ""
    unset border
    plot NaN notitle
}

