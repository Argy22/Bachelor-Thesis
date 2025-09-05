set terminal pngcairo size 1200,600 enhanced font 'Arial,12'
set output "throughput_bars.png"
set key outside right top

set title "TX/RX Throughput and Lost Packets per Scenario" font "Arial,16"
set xlabel "Scenario" font "Arial,14"
set ylabel "Throughput (Mbps)" font "Arial,14"
set y2label "Lost Packets" font "Arial,14"

set tics font "Arial,12"
set y2tics font "Arial,12"
set grid ytics
set style data histograms
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.9


$Data << EOD
#Scenario   TX      RX      LostPackets
1   0.263   0.263   0
2   0.264   0.087   2040
3   0.272   0.145   1543
4   0.290   0.071   2412
EOD


plot $Data using 2:xtic(1) title "TX Throughput" lc rgb "#0865d2", \
     '' using 3 title "RX Throughput" lc rgb "#d7db1b", \
     '' using 4 axes x1y2 title "Lost Packets" lc rgb "#e1211d"
