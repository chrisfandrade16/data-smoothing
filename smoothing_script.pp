set term png
set output "smoothing_plot.png"

plot "smoothing_original_data" with l title "original data" linetype 6, \
     "smoothing_smoothed_data" with l title "smoothed data" linetype 8, \