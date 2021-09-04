set datafile separator ","
set terminal png size 900,400
set title "Sensor reading with low and high thresholds"
set xlabel "Time (raw #)"
set ylabel "Raw value"
set nokey
set grid
#set mxtics
set xrange [3000:3700]
set arrow from graph 0,first 750 to graph 1,first 750 nohead lw 2 lt 0 lc rgb "blue" front
set arrow from graph 0,first 900 to graph 1,first 900 nohead lw 2 lt 0 lc rgb "red" front
set label "high" at graph 0.01,first 910 left tc rgb "red"
set label "low" at graph 0.01,first 760 left tc rgb "blue"
plot "ir.log" using 1 with lp lw 2 lt 2 lc rgb "black" title 'raw'
