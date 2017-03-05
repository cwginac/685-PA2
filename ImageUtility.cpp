#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void WriteImage(char fname[], vector <vector <int> > fimage, int M, int N, int Q)
{
	int i, j;

	unsigned char *image;

	ofstream fout;

	image = (unsigned char *) new unsigned char [M*M];

	for(i=0; i<N; i++)
	{
		for(j=0; j<M; j++)
		{
			image[i*M+j]=(unsigned char)fimage[i][j];
		}
	}

	fout.open(fname, ios::out);

	if (!fout) 
	{
		cout << "Can't open file: " << fname << endl;
		exit(1);
	}

	fout << "P5" << endl;
	fout << M << " " << N << endl;
	fout << Q << endl;

	fout.write( reinterpret_cast<char *>(image), (M*N)*sizeof(unsigned char));

	if (fout.fail()) 
	{
		cout << "Can't write image " << fname << endl;
		exit(0);
	}

	fout.close();
}

void ReadImage (char fname[], vector <vector <int> > &fimage, int& M, int& N, int& Q)
{
	int i, j;
	unsigned char *image;
	char header [100], *ptr;
	ifstream fin;
	fin.open(fname, ios::in);

	if (!fin)
	{
		cout << "Can't read image: " << fname << endl;
		exit (1);
	}

	// read header
	fin.getline(header,100,'\n');

	if ((header[0]!=80) ||
	  	(header[1]!=53)) 
	{ 
		cout << "Image " << fname << " is not PGM" << endl;
		exit(1);
	}

	fin.getline(header,100,'\n');
	while(header[0]=='#')
		fin.getline(header,100,'\n');

	M=strtol(header,&ptr,0);
	N=atoi(ptr);

	fin.getline(header,100,'\n');

	Q=strtol(header,&ptr,0);

	image = (unsigned char *) new unsigned char [M*N];

	fin.read( reinterpret_cast<char *>(image), (M*N)*sizeof(unsigned char));

	if (fin.fail()) 
	{
		cout << "Image " << fname << " has wrong size" << endl;
		exit(1);
	}

	fin.close();

	for (int a = 0; a < N; a++)
	{
		fimage.push_back (vector <int> ());
		for (int b = 0; b < M; b++)
		{
			fimage[a].push_back((int)image[a*M+b]);
		}
	}
}