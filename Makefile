build:
	g++ -I /usr/local/include/eigen3 smoothing_algorithm.cpp -o smoothing_executable.o

run_4_64:
	./smoothing_executable.o 4 64 smoothing_original_data smoothing_smoothed_data

run_2_3:
	./smoothing_executable.o 2 3 smoothing_original_data smoothing_smoothed_data


run_2_4:
	./smoothing_executable.o 2 4 smoothing_original_data smoothing_smoothed_data


run_4_5:
	./smoothing_executable.o 4 5 smoothing_original_data smoothing_smoothed_data


run_4_6:
	./smoothing_executable.o 4 6 smoothing_original_data smoothing_smoothed_data

plot:
	gnuplot smoothing_script.pp
