#include <iostream>
#include <fstream>
#include <math.h>

#include <Eigen/Dense>

#define MAX_INPUT_DATA_POINTS_LENGTH 2048
#define MAX_INPUT_DATA_POINT_LENGTH 20

using namespace std;
using namespace Eigen;

MatrixXd get_slice_of_matrix(int min_row, int min_col, int max_row, int max_col, MatrixXd matrix)
{
	MatrixXd slice_of_matrix = MatrixXd(max_row - min_row, max_col - min_col);

	for(int row = min_row; row < max_row; row++)
	{
		for(int col = min_col; min_col < max_col; max_col++)
		{
			slice_of_matrix(row - min_row, col - min_col) = matrix(row, col);
		}
	}

	return slice_of_matrix;
}

MatrixXd comp_coeff(int n, int m)
{
	double* z = (double*)(calloc(m, sizeof(double)));
	MatrixXd J = MatrixXd(m, n + 1);
	for (int i = 0; i < m; i++)
	{
		z[i] = ((i + 1) - (m - i)) / 2; 
		for (int j = 0; j < n + 1; j++)
		{
			J(i, j) = pow(z[i], j);
		}
	}

	MatrixXd J_transpose = J.transpose();
	MatrixXd first_step = J_transpose * J;
	MatrixXd second_step = first_step.inverse();
	MatrixXd third_step = second_step * J.transpose();

	return third_step;
}

VectorXd coeff_filter(MatrixXd coeff, int n, FILE* data, int m)
{
	VectorXd smoothed_y = VectorXd(MAX_INPUT_DATA_POINTS_LENGTH);
	char *input_file_line = (char*)calloc(MAX_INPUT_DATA_POINT_LENGTH, sizeof(char));
	int x_index = 1;
	int y_index = 0;

	// I wasn't sure how to get the smoothed output using the coefficients so I just did it the way I knew from tutorial using known_matrix * unknown_vector :(
	while(1)
	{
		MatrixXd known_matrix = MatrixXd(m, n + 1);
		for(int known_matrix_row = 0; known_matrix_row < m; known_matrix_row++)
		{
			for(int known_matrix_column = 0; known_matrix_column < n + 1; known_matrix_column++)
			{
				// 1, x_0, x_0^2...
				// 1, x_1, x_1^2...
				// x_i = [ 1, 2, 3, 4, 5 ...] (not given in data file so I'm generating the x values)
				known_matrix(known_matrix_row, known_matrix_column) = pow(x_index, known_matrix_column);
			}
			x_index++;
		}

		VectorXd actual_y_slice = VectorXd(m);
		for(int actual_y_slice_row = 0; actual_y_slice_row < m; actual_y_slice_row++)
		{
			// read given data file line by line
			if(fscanf(data, "%[^\n] ", input_file_line) == EOF)
			{
				free(input_file_line);
				smoothed_y.resize(y_index);
				return smoothed_y;
			};
			// actual_y_slice_row = [y1, y2, y3 ... y_m] from data file
			// actual_y_slice_row = [y_(m+1), y_(m+2), y_(m+3), ... , y_(2m)] on next while loop iteration, and so on
			actual_y_slice(actual_y_slice_row) = strtod(input_file_line, NULL);
		}

		// theta = (phi^T * phi)^-1 * phi^T * y
		VectorXd unknown_vector = (((known_matrix.transpose() * known_matrix).inverse()) * known_matrix.transpose()) * actual_y_slice;
		cout << (((known_matrix.transpose() * known_matrix).inverse()) * known_matrix.transpose()) << endl;
		cout << "==========================================" << endl;

		// Y = phi * theta
		VectorXd smoothed_y_slice = known_matrix * unknown_vector;

		for(int smoothed_y_slice_row = 0; smoothed_y_slice_row < m; smoothed_y_slice_row++)
		{
		    // save smoothed y slice to smoothed y total vector
			smoothed_y(y_index + smoothed_y_slice_row) = smoothed_y_slice(smoothed_y_slice_row);
		}

		y_index += m;
	}
}

int main(int argc, char **argv)
{
	if(argc > 5 || argc < 5) {
		cerr << "Needs exactly four command line arguments: polynomial degree, slice length, input data file name, and output data file name (in that order)" << endl;
		return 1;
	}

	int polynomial_degree = atoi(argv[1]);
	int slice_length = atoi(argv[2]);
	char* input_file_name = argv[3];
	char* output_file_name = argv[4];

	MatrixXd coefficients = comp_coeff(polynomial_degree, slice_length);

	FILE* input_file_pointer = fopen(input_file_name, "r");
    if (input_file_pointer == NULL)
	{
        cerr << "Input file with given name could not be found" << endl;
		return 1;
	}

	VectorXd smoothed_output = coeff_filter(coefficients, polynomial_degree, input_file_pointer, slice_length);

	fclose(input_file_pointer);

	ofstream output_file;
	output_file.open(output_file_name);
	output_file << smoothed_output << endl;
	output_file.close();

	cout << coefficients << endl;

	return 0;
}
