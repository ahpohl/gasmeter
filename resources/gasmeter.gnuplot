set datafile separator ","
set terminal png size 900,400
set title "Magnetometer"
set xdata time
set timefmt "%Y-%m-%d %H:%M:%S"
set format x "%H:%M"
set xlabel "Time (hh:mm)"
set ylabel "Magnetic field (10^{-7} T)"
set y2label "Volume (m^{3})"
#set key top right
unset key
set grid
set xrange ["2019-12-11 20:00":"2019-12-11 20:30"]
set y2tics

plot "../mag.log" using 1:5 axes x1y1 with lines lw 2 lt 2 lc rgb "black" title "b_{y}", \
  '' using 1:($3*0.01) axes x1y2 with lines lw 2 lt 2 lc rgb "blue" title "count"
