set datafile separator ","
set terminal png size 900,400
set title "Magnetometer"
set xdata time
set timefmt "%s"
set format x "%H:%M"
set xlabel "Time (hh:mm)"
set ylabel "Magnetic field (10^{-7} T)"
set key top right
set grid
#set xrange ["09:30":"09:35"]
plot "mag.txt" using ($1+7200):2 with lines lw 2 lt 2 lc rgb "black" title 'B', \
"mag.txt" using ($1+7200):3 with lines lw 2 lt 2 lc rgb "red" title 'B_{x}', \
"mag.txt" using ($1+7200):4 with lines lw 2 lt 2 lc rgb "blue" title 'B_{y}', \
"mag.txt" using ($1+7200):5 with lines lw 2 lt 2 lc rgb "cyan" title 'B_{z}'
