set datafile separator ","
#set terminal png size 900,400
set term wxt size 1200,600
set title "Magnetometer"
set xdata time
set timefmt "%Y-%m-%d %H:%M:%S"
set format x "%H:%M"
set xlabel "Time (hh:mm)"
set ylabel "Magnetic field (10^{-7} T)"
set y2label "Volume (m^{3})"
set key top right
unset key
set grid
#set xrange ["2020-02-01 17:30":"2020-02-01 17:35"]
set y2tics

plot "< tail -n 900 ../mag.log" using 1:4 axes x1y1 with lines lw 2 lt 2 lc rgb "black" title "b_{y}", \
  "< tail -n 5 ../count.log" using 1:3 axes x1y2 with points lw 2 lt 2 lc rgb "blue" title "count"

pause 5
reread
replot
