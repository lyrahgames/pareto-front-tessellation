set size square
unset key
set lmargin 2
set rmargin 1
set tmargin 1
set bmargin 2

set xtics 0.2
set ytics 0.4

set format x "$%.1f$"
set format y "$%.1f$"

plot 'zdt3_1000_745_1000.dat' u 1:2 w lines lc rgb "#222222" lw 5