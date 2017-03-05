#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "ImageUtility.cpp"

using namespace std;

extern "C" void solve_system(int, int, float**, float*, float*); 

void findInitialAffineTransformation (float *x);
void transformImage (float **a, float b1, float b2);

int main (int argc, char* argv[])
{
	int m = 8;
	int n = 6;

	// Original picture
	// left eye
	float x1 = 27;
	float y1 = 51;

	// right eye
	float x2 = 64;
	float y2 = 50;

	// nose
	float x3 = 46;
	float y3 = 71;

	// mouth
	float x4 = 46;
	float y4 = 88;

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

	//b = new float [m + 1];
	float *b = new float [9] {-1, x1f, y1f, x2f, y2f, x3f, y3f, x4f, y4f};

	solve_system (m, n, a, x, b);

	float **newA = new float* [3];
	newA[1] = new float [3] {-1, x [1], x [2]};
	newA[2] = new float [3] {-1, x [4], x [5]};

	float b1 = x[3];
	float b2 = x[6];

	transformImage (newA, b1, b2);
}

void transformImage (float **a, float b1, float b2)
{
	char inputFileName [] = "1.pbm";
	char outputFileName [] = "1trans.pbm";
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