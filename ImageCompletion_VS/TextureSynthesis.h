#ifndef _TEXTURESYNTHESIS_H
#define _TEXTURESYNTHESIS_H

#include <opencv2/opencv.hpp>
using namespace cv;

#include <iostream>
#include <fstream>

using namespace std;

#define M_PI 3.14

#define MAX 100

class TextureSynthesis
{

private:
	int x_y[4];

	int x_of_struct = 0;
	int y_of_struct = 0;


public:
	TextureSynthesis(){}
	~TextureSynthesis(){}
	void getTexture(Mat source, Mat mask);
//	Mat getTexture(Mat source, Mat mask, int size);
	Mat Compare_T(Mat srcMat, Mat core, int x1, int y1, int x2, int y2);
	void Calculate_map(int jx, int iy, int nx, int ny, int xmap[], int ymap[]);
};

#endif