set size square
unset key
set lmargin 4.5
set rmargin 1.5
set tmargin 0
set bmargin 0

set xtics 0.5
# set ytics 2

set xrange [0:1]
set yrange [0.001:0.5]

set format x "$%.1f$"
set format y "$10^{%T}$"
set logscale y

plot 'kursawe_histogram_1000.dat' w boxes lc black fs solid 0.2