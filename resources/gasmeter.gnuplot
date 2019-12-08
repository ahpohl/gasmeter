set datafile separator ","
set terminal png size 900,400
set title "Magnetometer"
set xdata time
set timefmt "%Y-%m-%d %H:%M:%S"
set format x "%H:%M"
set xlabel "Time (hh:mm)"
set ylabel "Magnetic field (10^{-7} T)"
set key top right
set grid
set xrange ["2019-12-08 18:00":"2019-12-08 18:300"]
plot "../mag.log" using 1:3 with lines lw 2 lt 2 lc rgb "red" title 'B_{x}', \
 "../mag.log" using 1:4 with lines lw 2 lt 2 lc rgb "blue" title 'B_{y}', \
 "../mag.log" using 1:5 with lines lw 2 lt 2 lc rgb "cyan" title 'B_{z}'
