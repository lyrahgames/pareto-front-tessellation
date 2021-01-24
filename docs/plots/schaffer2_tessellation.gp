set size square
unset key
set lmargin 2
set rmargin 1
set tmargin 1
set bmargin 2

set xtics 0.5
set ytics 5

# set format x "$%.1f$"
# set format y "$%.1f$"

plot 'schaffer2_1000_tessellation.dat' u 1:2 w lines lw 5 lc rgb "#222222"