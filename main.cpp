#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "ImageUtility.cpp"

using namespace std;

extern "C" void solve_system(int, int, float**, float*, float*); 

void findInitialAffineTransformation (float *x);
void transformImage (char inputFileName [], char outputFileName [], float **a, float b1, float b2);
void correctLighting (char inputFileName [], char outputFileName [], char parametersFileName []);

int main (int argc, char* argv[])
{
	int m = 8;
	int n = 6;

	ifstream fin ("originalLocations.txt");

	if (!fin.is_open()) 
	{
		cout << "file not found" << endl;
		return 0;
	}

	ofstream fout ("affineTransformations.txt");
	fout << "a11, \ta12, \tb1, \ta21, \ta22, \tb2, \terror" << endl;

	string value;

	char inputFileName [10];
	char outputFileName [15];
	char correctedFileName [20];
	char lightingParameters [20];

	for (int i = 1; i <= 10; i++)
	{
		char number [3];
		sprintf (number, "%d", i);

		strcpy (inputFileName, number);
		strcat (inputFileName, ".pbm");

		strcpy (outputFileName, number);
		strcat (outputFileName, "output.pbm");

		strcpy (correctedFileName, number);
		strcat (correctedFileName, "corrected.pbm");

		strcpy (lightingParameters, number);
		strcat (lightingParameters, "lightingParameters.txt");

		// Original picture
		// left eye
		getline(fin, value, ',');
		float x1 = atof (value.c_str());
		getline(fin, value, ',');
		float y1 = atof (value.c_str());

		// right eye
		getline(fin, value, ',');
		float x2 = atof (value.c_str());
		getline(fin, value, ',');
		float y2 = atof (value.c_str());

		// nose
		getline(fin, value, ',');
		float x3 = atof (value.c_str());
		getline(fin, value, ',');
		float y3 = atof (value.c_str());

		// mouth
		getline(fin, value, ',');
		float x4 = atof (value.c_str());
		getline(fin, value, ',');
		float y4 = atof (value.c_str());

		// New picture
		// left eye
		float x1f = 13;
		float y1f = 16;

		// right eye
		float x2f = 27;
		float y2f = 16;

		// nose
		float x3f = 20;
		float y3f = 24;

		// mouth
		float x4f = 20;
		float y4f = 36;

		float **a = new float* [m + 1];
		a[1] = new float [7] {-1, x1, y1, 1, 0, 0, 0};
		a[2] = new float [7] {-1, 0, 0, 0, x1, y1, 1};
		a[3] = new float [7] {-1, x2, y2, 1, 0, 0, 0};
		a[4] = new float [7] {-1, 0, 0, 0, x2, y2, 1};
		a[5] = new float [7] {-1, x3, y3, 1, 0, 0, 0};
		a[6] = new float [7] {-1, 0, 0, 0, x3, y3, 1};
		a[7] = new float [7] {-1, x4, y4, 1, 0, 0, 0};
		a[8] = new float [7] {-1, 0, 0, 0, x4, y4, 1};

		float *x = new float [n + 1];

		float *b = new float [9] {-1, x1f, y1f, x2f, y2f, x3f, y3f, x4f, y4f};

		solve_system (m, n, a, x, b);

		// Get error
		float error = 0;

		float x1t = (x[1] * x1) + (x[2] * y1) + x[3];
		float y1t = (x[4] * x1) + (x[5] * y1) + x[6];
		error += sqrt ((x1t-x1f)*(x1t-x1f) + (y1t-y1f)*(y1t-y1f));

		float x2t = (x[1] * x2) + (x[2] * y2) + x[3];
		float y2t = (x[4] * x2) + (x[5] * y2) + x[6];
		error += sqrt ((x2t-x2f)*(x2t-x2f) + (y2t-y2f)*(y2t-y2f));

		float x3t = (x[1] * x3) + (x[2] * y3) + x[3];
		float y3t = (x[4] * x3) + (x[5] * y3) + x[6];
		error += sqrt ((x3t-x3f)*(x3t-x3f) + (y3t-y3f)*(y3t-y3f));

		float x4t = (x[1] * x4) + (x[2] * y4) + x[3];
		float y4t = (x[4] * x4) + (x[5] * y4) + x[6];
		error += sqrt ((x4t-x4f)*(x4t-x4f) + (y4t-y4f)*(y4t-y4f));

		error /= 4.0;

		// Populate new array for transform image
		float **newA = new float* [3];
		newA[1] = new float [3] {-1, x [1], x [2]};
		newA[2] = new float [3] {-1, x [4], x [5]};

		float b1 = x[3];
		float b2 = x[6];

		transformImage (inputFileName, outputFileName, newA, b1, b2);

		// Output Data
		fout << x[1] << ", " << x[2] << ", " << x[3] << ", ";
		fout << x[4] << ", " << x[5] << ", " << x[6] << ", " << error << endl;

		//correctLighting (outputFileName, correctedFileName, lightingParameters);
	}
}

void transformImage (char inputFileName [], char outputFileName [], float **a, float b1, float b2)
{
	vector <vector <int> > image;
	int M, N, Q;

	ReadImage (inputFileName, image, M, N, Q);


	vector <vector <int> > outputImage;

	// Y
	for (int i = 0; i < 48; i++)
	{
		outputImage.push_back (vector <int> ());
		// X
		for (int c = 0; c < 40; c++)
		{
			float *x = new float [3];
			float *b = new float [3] {-1, (float)c - b1, (float)i - b2};

			solve_system (2, 2, a, x, b);

			if ((int)x[1] >= 0 && (int)x[1] < image[0].size() && (int)x[2] >= 0 && (int)x[2] < image.size())
			{
				outputImage[i].push_back (image [(int) x[2]][(int) x[1]]);
			}
			else
			{
				outputImage[i].push_back (0);
			}

			delete [] x;
			delete [] b;
		}
	}

	WriteImage (outputFileName, outputImage, 40, 48, Q);
}

void correctLighting (char inputFileName [], char outputFileName [], char parametersFileName[])
{
	vector <vector <int> > image;
	int M, N, Q;

	ReadImage (inputFileName, image, M, N, Q);

	float **a = new float* [(M * N) + 1];
	float *b = new float [(M * N) + 1];

	for (int x = 0; x < M; x++)
	{
		for (int y = 0; y < N; y++) 
		{
			// {-1, x, y, x*y, 1}
			a [(x * M) + y + 1] = new float [5] {-1, (float)x, (float)y, (float)(x * y), 1};
			b [(x * M) + y + 1] = image [x][y];
		}
	}

	// {-1, a, b, c, d}
	float *x = new float [5];

	solve_system (M, N, a, x, b);

	vector <vector <int> > outputImage;
	outputImage = image;

	for (int c = 0; c < N; c++)
	{
		for (int i = 0; i < M; i++)
		{
			outputImage[c][i] -= (c * x[1] + i * x[2] + c*i*x[3] + x[4]);
		}
	}

	WriteImage (outputFileName, outputImage, M, N, Q);

	ofstream fout (parametersFileName);
	fout << x[1] << ", " << x[2] << ", " << x[3] << ", " << x[4] << endl;
	fout.close();
}