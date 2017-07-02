#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <ctime>
#include <math.h>
#include "anchor.h"
using namespace std;

#include "TextureSynthesis.h"

#define randn(x) (rand()%(x))

int x[MAX] = {0};
int y[MAX] = {0};

double natGaussian(double x, double sigma = 1.0, double mu = 0.0)
{
	return exp(-(x - mu)*(x - mu) / (2 * sigma*sigma)) / (sigma*sqrt(2 * M_PI));
}

int pow_sum(int x, int y, int z)
{
	return pow(x, 2) + pow(y, 2) + pow(z, 2);
}

void TextureSynthesis::getTexture(Mat srcMat, Mat mask)
{
	cout << "Texture Synthesis begin!" << endl;
	Mat src = srcMat.clone();
	Mat m = mask.clone();

	int height = srcMat.size().height;
	int width = srcMat.size().width;

//	cout << "width:" << width << endl;
//	cout << "height:" << height << endl;

	Mat d_value = m.clone();
//	imwrite("DV.jpg", d_value);

	int left = width;
	int right = 0;
	int top = height; 
	int buttom = 0;

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (d_value.at<Vec3b>(i, j)[0]>100 && d_value.at<Vec3b>(i, j)[1]<20 && d_value.at<Vec3b>(i, j)[2]<20)
			{
				d_value.at<Vec3b>(i, j)[0] = 255;
				d_value.at<Vec3b>(i, j)[1] = 255;
				d_value.at<Vec3b>(i, j)[2] = 255;
				d_value.at<Vec3b>(i, j - 1)[0] = 255;
				d_value.at<Vec3b>(i, j - 1)[1] = 255;
				d_value.at<Vec3b>(i, j - 1)[2] = 255;
				d_value.at<Vec3b>(i - 1, j)[0] = 255;
				d_value.at<Vec3b>(i - 1, j)[1] = 255;
				d_value.at<Vec3b>(i - 1, j)[2] = 255;
				d_value.at<Vec3b>(i, j + 1)[0] = 255;
				d_value.at<Vec3b>(i, j + 1)[1] = 255;
				d_value.at<Vec3b>(i, j + 1)[2] = 255;
				d_value.at<Vec3b>(i + 1, j)[0] = 255;
				d_value.at<Vec3b>(i + 1, j)[1] = 255;
				d_value.at<Vec3b>(i + 1, j)[2] = 255;
				d_value.at<Vec3b>(i - 1, j - 1)[0] = 255;
				d_value.at<Vec3b>(i - 1, j - 1)[1] = 255;
				d_value.at<Vec3b>(i - 1, j - 1)[2] = 255;
				d_value.at<Vec3b>(i - 1, j + 1)[0] = 255;
				d_value.at<Vec3b>(i - 1, j + 1)[1] = 255;
				d_value.at<Vec3b>(i - 1, j + 1)[2] = 255;
				d_value.at<Vec3b>(i - 1, j + 1)[0] = 255;
				d_value.at<Vec3b>(i - 1, j + 1)[1] = 255;
				d_value.at<Vec3b>(i - 1, j + 1)[2] = 255;
				d_value.at<Vec3b>(i + 1, j + 1)[0] = 255;
				d_value.at<Vec3b>(i + 1, j + 1)[1] = 255;
				d_value.at<Vec3b>(i + 1, j + 1)[2] = 255;


				if (j < left) left = j;
				if (j > right) right = j;
				if (i < top) top = i;
				if (i > buttom) buttom = i;
			}
			else
			{
				d_value.at<Vec3b>(i, j)[0] = 0;
				d_value.at<Vec3b>(i, j)[1] = 0;
				d_value.at<Vec3b>(i, j)[2] = 0;
			}
		}
	}

	imwrite("d_value.jpg", d_value);
//	imshow("d_value", d_value);
	cout << "width:" << d_value.size().width << " height:" << d_value.size().height << endl;
	cout << "left:" << left << " right:" << right << " top:" << top << " buttom:" << buttom << endl;

	cout << endl;

	y[0] = top;
	x[0] = left;

	cout << "y[0]" << y[0] << " x[0]" << x[0] << endl;
	cout << endl;


	for (int i = top; i <= buttom; i++)
	{
		for (int j = left; j <= right; j++)
		{

			if (d_value.at<Vec3b>(i, j)[0] == 255)
			{
				if (d_value.at<Vec3b>(i, j - 1)[0] == 255 && d_value.at<Vec3b>(i, j + 1)[0] == 0)
				{
					if (x_of_struct == 0)
					{
						x_of_struct++;
						x[x_of_struct] = j;
					}
					else
					{
						if (j > x[x_of_struct] && (j - x[x_of_struct]) <= 5)
							x[x_of_struct] = j;
						if ((j - x[x_of_struct]) > 5)
						{
							x_of_struct++;
							x[x_of_struct] = j;
							if (j == right)
							{
								break;
							}
						}
					}
				}
			}
			if (d_value.at<Vec3b>(i, j)[0] == 0)
				continue;
		}
	}

	x_of_struct++;
	x[x_of_struct] = right;

	for (int i = top; i <= buttom; i++)
	{
		for (int j = left; j <= right; j++)
		{

			if (d_value.at<Vec3b>(i, j)[0] == 255)
			{
				if (d_value.at<Vec3b>(i - 1, j)[0] == 255 && d_value.at<Vec3b>(i + 1, j)[0] == 0)
				{
					if (y_of_struct == 0)
					{
						y_of_struct++;
						y[y_of_struct] = i;
					}
					else
					{
						if (i > y[y_of_struct] && (i - y[y_of_struct]) <= 5)
							y[y_of_struct] = i;
						if ((i - y[y_of_struct]) > 6)
						{
							y_of_struct++;
							y[y_of_struct] = i;
							if (i == buttom)
							{
								break;
							}
						}
					}
				}
			}
			if (d_value.at<Vec3b>(i, j)[0] == 0)
				continue;
		}
	}
	y_of_struct++;
	y[y_of_struct] = buttom;

	cout << "x" << endl;
	for (int i = 0; i < x_of_struct; i++)
		cout << x[i] << endl;

	cout << "y" << endl;
	for (int i = 0; i < y_of_struct; i++)
		cout << y[i] << endl;
	cout << endl;

	Mat temp = mask.clone();

	int n = 0;

	for (int i = top; i < buttom; i++)
	{
		uchar *t_data = temp.ptr<uchar>(i);
		uchar *d_data = d_value.ptr<uchar>(i);
		for (int j = left; j < right; j++)
		{
			if (d_value.at<Vec3b>(i, j)[0] == 0)
			{
//				n++;
				continue;
			}
				
			else
			{
				if (d_value.at<Vec3b>(i, j)[0] == 255)
				{
					n++;
//					cout << "No:" << n << endl;
//					cout << endl;

//					cout << "j: " << j << " i: " << i << endl;
					Calculate_map(j, i, x_of_struct, y_of_struct, x, y);

					int x1 = x_y[0];
					int x2 = x_y[1];
					int y1 = x_y[2];
					int y2 = x_y[3];

//					cout << "x1:" << x1 << " y1:" << y1 << " x2:" << x2 << " y2:" << y2 << endl;

					Mat core = Mat::zeros(3, 3, CV_8UC3);

					uchar *core_data1 = core.ptr<uchar>(0);
					uchar *core_data2 = core.ptr<uchar>(1);
					uchar *core_data3 = core.ptr<uchar>(2);

					uchar* t_data0 = temp.ptr<uchar>(i - 1);
					uchar* t_data2 = temp.ptr<uchar>(i + 1);

					if (y2 != y[y_of_struct])
					{
						core_data1[0] = t_data0[j - 1];
						core_data1[1] = t_data0[j];
						core_data1[2] = t_data0[j + 1];
					}
					else
					{
						core_data3[0] = t_data2[j - 1];
						core_data3[1] = t_data2[j];
						core_data3[2] = t_data2[j + 1];
					}
					if (x2 != x[x_of_struct])
						core_data2[0] = t_data[j - 1];
					else
						core_data2[2] = t_data[j + 1];

//					cout << "i:" << i << " j:" << j << endl;

					Mat c = Compare_T(src, core, x1, y1, x2, y2);

					temp.at<Vec3b>(i, j)[0] = c.at<Vec3b>(1, 1)[0];
					temp.at<Vec3b>(i, j)[1] = c.at<Vec3b>(1, 1)[1];
					temp.at<Vec3b>(i, j)[2] = c.at<Vec3b>(1, 1)[2];
				}

			}
		}
	}
	
	imwrite("result.jpg", temp);

	cout << "end" << endl;

}


void TextureSynthesis::Calculate_map(int jx, int iy, int nx, int ny, int xmap[], int ymap[])
{
	for (int i = 0; i < ny; i++)
	{
		if (iy >= ymap[i] && iy <= ymap[i + 1])
		{
			x_y[2] = ymap[i];
			x_y[3] = ymap[i + 1];
		}
	}

	for (int j = 0; j < nx; j++)
	{
		if (jx >= xmap[j] && jx <= xmap[j + 1])
		{
			x_y[0] = xmap[j];
			x_y[1] = xmap[j + 1];
		}
	}

}

Mat TextureSynthesis::Compare_T(Mat srcMat, Mat core, int x1, int y1, int x2, int y2)
{
	Mat src = srcMat.clone();
	Mat c = core.clone();

	double sum = 0;
	double min = 1E20;
	int s = 0;
	int t = 0;


	int cb1 = c.at<Vec3b>(0, 0)[0];
	int cg1 = c.at<Vec3b>(0, 0)[1];
	int cr1 = c.at<Vec3b>(0, 0)[2];
	int cb2 = c.at<Vec3b>(0, 1)[0];
	int cg2 = c.at<Vec3b>(0, 1)[1];
	int cr2 = c.at<Vec3b>(0, 1)[2];
	int cb3 = c.at<Vec3b>(0, 2)[0];
	int cg3 = c.at<Vec3b>(0, 2)[1];
	int cr3 = c.at<Vec3b>(0, 2)[2];
	int cb4 = c.at<Vec3b>(1, 0)[0];
	int cg4 = c.at<Vec3b>(1, 0)[1];
	int cr4 = c.at<Vec3b>(1, 0)[2];

	int cb6 = c.at<Vec3b>(1, 2)[0];
	int cg6 = c.at<Vec3b>(1, 2)[1];
	int cr6 = c.at<Vec3b>(1, 2)[2];
	int cb7 = c.at<Vec3b>(2, 0)[0];
	int cg7 = c.at<Vec3b>(2, 0)[1];
	int cr7 = c.at<Vec3b>(2, 0)[2];
	int cb8 = c.at<Vec3b>(2, 1)[0];
	int cg8 = c.at<Vec3b>(2, 1)[1];
	int cr8 = c.at<Vec3b>(2, 1)[2];
	int cb9 = c.at<Vec3b>(2, 2)[0];
	int cg9 = c.at<Vec3b>(2, 2)[1];
	int cr9 = c.at<Vec3b>(2, 2)[2];


	if (x2 != x[x_of_struct] && y2 != y[y_of_struct])
	{
//		cout << "MODE 1" << endl;
		for (int i = y1 - PatchSizeRow / 2; i < y1; i++)
		{
			for (int j = x1 - PatchSizeRow / 2; j < x1; j++)
			{
				int sb1 = src.at<Vec3b>(i, j)[0];
				int sg1 = src.at<Vec3b>(i, j)[1];
				int sr1 = src.at<Vec3b>(i, j)[2];
				int sb2 = src.at<Vec3b>(i, j + 1)[0];
				int sg2 = src.at<Vec3b>(i, j + 1)[1];
				int sr2 = src.at<Vec3b>(i, j + 1)[2];
				int sb3 = src.at<Vec3b>(i, j + 2)[0];
				int sg3 = src.at<Vec3b>(i, j + 2)[1];
				int sr3 = src.at<Vec3b>(i, j + 2)[2];
				int sb4 = src.at<Vec3b>(i + 1, j)[0];
				int sg4 = src.at<Vec3b>(i + 1, j)[1];
				int sr4 = src.at<Vec3b>(i + 1, j)[2];

				sum = pow_sum(cb1 - sb1, cg1 - sg1, cr1 - sr1) + pow_sum(cb2 - sb2, cg2 - sg2, cr2 - sr2)
					+ pow_sum(cb3 - sb3, cg3 - sg3, cr3 - sr3) + pow_sum(cb4 - sb4, cg4 - sg4, cr4 - sr4);
				//			cout << "sum:" << sum << endl;

				if (min > sum)
				{
					min = sum;
					s = i;
					t = j;
				}
			}
		}
	}

	else if (x2 == x[x_of_struct] && y2 != y[y_of_struct])
	{
//		cout << "MODE 2" << endl;
		for (int i = y1 - PatchSizeRow / 2; i < y1; i++)
		{
			for (int j = x2 + PatchSizeRow / 2; j > x2; j--)
			{
				int sb1 = src.at<Vec3b>(i, j)[0];
				int sg1 = src.at<Vec3b>(i, j)[1];
				int sr1 = src.at<Vec3b>(i, j)[2];
				int sb2 = src.at<Vec3b>(i, j + 1)[0];
				int sg2 = src.at<Vec3b>(i, j + 1)[1];
				int sr2 = src.at<Vec3b>(i, j + 1)[2];
				int sb3 = src.at<Vec3b>(i, j + 2)[0];
				int sg3 = src.at<Vec3b>(i, j + 2)[1];
				int sr3 = src.at<Vec3b>(i, j + 2)[2];
				int sb4 = src.at<Vec3b>(i + 1, j + 2)[0];
				int sg4 = src.at<Vec3b>(i + 1, j + 2)[1];
				int sr4 = src.at<Vec3b>(i + 1, j + 2)[2];

				sum = pow_sum(cb1 - sb1, cg1 - sg1, cr1 - sr1) + pow_sum(cb2 - sb2, cg2 - sg2, cr2 - sr2)
					+ pow_sum(cb3 - sb3, cg3 - sg3, cr3 - sr3) + pow_sum(cb6 - sb4, cg6 - sg4, cr6 - sr4);
				//			cout << "sum:" << sum << endl;

				if (min > sum)
				{
					min = sum;
					s = i;
					t = j;
				}
			}
		}
	}

	else if (x2 != x[x_of_struct] && y2 == y[y_of_struct])
	{
//		cout << "MODE 4" << endl;
		for (int i = y2 + PatchSizeRow / 2; i > y2; i--)
		{
			for (int j = x1 - PatchSizeRow / 2; j < x1; j++)
			{
				int sb1 = src.at<Vec3b>(i + 1, j)[0];
				int sg1 = src.at<Vec3b>(i + 1, j)[1];
				int sr1 = src.at<Vec3b>(i + 1, j)[2];
				int sb2 = src.at<Vec3b>(i + 2, j)[0];
				int sg2 = src.at<Vec3b>(i + 2, j)[1];
				int sr2 = src.at<Vec3b>(i + 2, j)[2];
				int sb3 = src.at<Vec3b>(i + 2, j + 1)[0];
				int sg3 = src.at<Vec3b>(i + 2, j + 1)[1];
				int sr3 = src.at<Vec3b>(i + 2, j + 1)[2];
				int sb4 = src.at<Vec3b>(i + 2, j + 2)[0];
				int sg4 = src.at<Vec3b>(i + 2, j + 2)[1];
				int sr4 = src.at<Vec3b>(i + 2, j + 2)[2];

				sum = pow_sum(cb4 - sb1, cg4 - sg1, cr4 - sr1) + pow_sum(cb7 - sb2, cg7 - sg2, cr7 - sr2)
					+ pow_sum(cb8 - sb3, cg8 - sg3, cr8 - sr3) + pow_sum(cb9 - sb4, cg9 - sg4, cr9 - sr4);
				//			cout << "sum:" << sum << endl;

				if (min > sum)
				{
					min = sum;
					s = i;
					t = j;
				}
			}
		}
	}


	else if (x2 == x[x_of_struct] && y2 == y[y_of_struct])
	{
//		cout << "MODE 3" << endl;
		for (int i = y2 + PatchSizeRow / 2; i > y2; i--)
		{
			for (int j = x2 + PatchSizeRow / 2; j > x2; j--)
			{
				int sb1 = src.at<Vec3b>(i + 1, j + 2)[0];
				int sg1 = src.at<Vec3b>(i + 1, j + 2)[1];
				int sr1 = src.at<Vec3b>(i + 1, j + 2)[2];
				int sb2 = src.at<Vec3b>(i + 2, j)[0];
				int sg2 = src.at<Vec3b>(i + 2, j)[1];
				int sr2 = src.at<Vec3b>(i + 2, j)[2];
				int sb3 = src.at<Vec3b>(i + 2, j + 1)[0];
				int sg3 = src.at<Vec3b>(i + 2, j + 1)[1];
				int sr3 = src.at<Vec3b>(i + 2, j + 1)[2];
				int sb4 = src.at<Vec3b>(i + 2, j + 2)[0];
				int sg4 = src.at<Vec3b>(i + 2, j + 2)[1];
				int sr4 = src.at<Vec3b>(i + 2, j + 2)[2];

				sum = pow_sum(cb6 - sb1, cg6 - sg1, cr6 - sr1) + pow_sum(cb7 - sb2, cg7 - sg2, cr7 - sr2)
					+ pow_sum(cb8 - sb3, cg8 - sg3, cr8 - sr3) + pow_sum(cb9 - sb4, cg9 - sg4, cr9 - sr4);
				//			cout << "sum:" << sum << endl;

				if (min > sum)
				{
					min = sum;
					s = i;
					t = j;
				}
			}
		}
	}
	/*
	for (int i = y1; i < y2; i++)
	{
		for (int j = x1; j < x2; j++)
		{
			int sb1 = src.at<Vec3b>(i, j)[0];
			int sg1 = src.at<Vec3b>(i, j)[1];
			int sr1 = src.at<Vec3b>(i, j)[2];
			int sb2 = src.at<Vec3b>(i, j + 1)[0];
			int sg2 = src.at<Vec3b>(i, j + 1)[1];
			int sr2 = src.at<Vec3b>(i, j + 1)[2];
			int sb3 = src.at<Vec3b>(i, j + 2)[0];
			int sg3 = src.at<Vec3b>(i, j + 2)[1];
			int sr3 = src.at<Vec3b>(i, j + 2)[2];
			int sb4 = src.at<Vec3b>(i + 1, j)[0];
			int sg4 = src.at<Vec3b>(i + 1, j)[1];
			int sr4 = src.at<Vec3b>(i + 1, j)[2];

			sum = pow_sum(cb1 - sb1, cg1 - sg1, cr1 - sr1) + pow_sum(cb2 - sb2, cg2 - sg2, cr2 - sr2)
			+ pow_sum(cb3 - sb3, cg3 - sg3, cr3 - sr3) + pow_sum(cb4 - sb4, cg4 - sg4, cr4 - sr4);
			//			cout << "sum:" << sum << endl;

			if (min > sum)
			{
				min = sum;
				x = i;
				y = j;
			}
		}
	}
	*/

	c.at<Vec3b>(1, 1)[0] = src.at<Vec3b>(s + 1, t + 1)[0];
	c.at<Vec3b>(1, 1)[1] = src.at<Vec3b>(s + 1, t + 1)[1];
	c.at<Vec3b>(1, 1)[2] = src.at<Vec3b>(s + 1, t + 1)[2];

	return c;
}

/*
Mat TextureSynthesis::Compare_T(Mat srcMat, Mat core, int jx, int iy, int nx, int ny, int *xmap, int *ymap)
{
	Mat src = srcMat.clone();
	Mat c = core.clone();

	double sum = 0;
	double min = 1E20;
	int x = 0;
	int y = 0;

	int cb1 = c.at<Vec3b>(0, 0)[0];
	int cg1 = c.at<Vec3b>(0, 0)[1];
	int cr1 = c.at<Vec3b>(0, 0)[2];
	int cb2 = c.at<Vec3b>(0, 1)[0];
	int cg2 = c.at<Vec3b>(0, 1)[1];
	int cr2 = c.at<Vec3b>(0, 1)[2];
	int cb3 = c.at<Vec3b>(0, 2)[0];
	int cg3 = c.at<Vec3b>(0, 2)[1];
	int cr3 = c.at<Vec3b>(0, 2)[2];
	int cb4 = c.at<Vec3b>(1, 0)[0];
	int cg4 = c.at<Vec3b>(1, 0)[1];
	int cr4 = c.at<Vec3b>(1, 0)[2];

	int cb6 = c.at<Vec3b>(1, 2)[0];
	int cg6 = c.at<Vec3b>(1, 2)[1];
	int cr6 = c.at<Vec3b>(1, 2)[2];
	int cb7 = c.at<Vec3b>(2, 0)[0];
	int cg7 = c.at<Vec3b>(2, 0)[1];
	int cr7 = c.at<Vec3b>(2, 0)[2];
	int cb8 = c.at<Vec3b>(2, 1)[0];
	int cg8 = c.at<Vec3b>(2, 1)[1];
	int cr8 = c.at<Vec3b>(2, 1)[2];
	int cb9 = c.at<Vec3b>(2, 2)[0];
	int cg9 = c.at<Vec3b>(2, 2)[1];
	int cr9 = c.at<Vec3b>(2, 2)[2];
	

	int x1, x2, y1, y2;
	x1 = x2 = y1 = y2 = 0;

	for (int i = 0; i < ny-1; i++)
	{
		if (iy >= ymap[i] && iy <= ymap[i + 1])
		{
			y1 = ymap[i];
			y2 = ymap[i + 1];
		}
	}

	for (int j = 0; j < nx - 1; j++)
	{
		if (jx >= xmap[j] && jx <= xmap[j + 1])
		{
			x1 = xmap[j];
			x2 = xmap[j + 1];
		}
	}

//	cout << "x1:" << x1 << " x2:" << x2 << " y1:" << y1 << " y2:" << y2 << endl;

	if (x1 == xmap[0])
	{
		if (y1 == ymap[0])
		{
			for (int i = y1; i < y2; i++)
			{
				for (int j = x1; j < x2; j++)
				{
					int sb1 = src.at<Vec3b>(i, j)[0];
					int sg1 = src.at<Vec3b>(i, j)[1];
					int sr1 = src.at<Vec3b>(i, j)[2];
					int sb2 = src.at<Vec3b>(i, j + 1)[0];
					int sg2 = src.at<Vec3b>(i, j + 1)[1];
					int sr2 = src.at<Vec3b>(i, j + 1)[2];
					int sb3 = src.at<Vec3b>(i, j + 2)[0];
					int sg3 = src.at<Vec3b>(i, j + 2)[1];
					int sr3 = src.at<Vec3b>(i, j + 2)[2];
					int sb4 = src.at<Vec3b>(i + 1, j)[0];
					int sg4 = src.at<Vec3b>(i + 1, j)[1];
					int sr4 = src.at<Vec3b>(i + 1, j)[2];

					sum = pow_sum(cb1 - sb1, cg1 - sg1, cr1 - sr1) + pow_sum(cb2 - sb2, cg2 - sg2, cr2 - sr2)
						+ pow_sum(cb3 - sb3, cg3 - sg3, cr3 - sr3) + pow_sum(cb4 - sb4, cg4 - sg4, cr4 - sr4);
					//			cout << "sum:" << sum << endl;

					if (min > sum)
					{
						min = sum;
						x = i;
						y = j;
					}
				}
			}
		}

		else
		{

			for (int i = y1; i < y2; i++)
			{
				for (int j = x1; j < x2; j++)
				{
					int sb1 = src.at<Vec3b>(i, j)[0];
					int sg1 = src.at<Vec3b>(i, j)[1];
					int sr1 = src.at<Vec3b>(i, j)[2];
					int sb2 = src.at<Vec3b>(i, j + 1)[0];
					int sg2 = src.at<Vec3b>(i, j + 1)[1];
					int sr2 = src.at<Vec3b>(i, j + 1)[2];
					int sb3 = src.at<Vec3b>(i, j + 2)[0];
					int sg3 = src.at<Vec3b>(i, j + 2)[1];
					int sr3 = src.at<Vec3b>(i, j + 2)[2];
					int sb4 = src.at<Vec3b>(i + 1, j)[0];
					int sg4 = src.at<Vec3b>(i + 1, j)[1];
					int sr4 = src.at<Vec3b>(i + 1, j)[2];

					sum = pow_sum(cb1 - sb1, cg1 - sg1, cr1 - sr1) + pow_sum(cb2 - sb2, cg2 - sg2, cr2 - sr2)
						+ pow_sum(cb3 - sb3, cg3 - sg3, cr3 - sr3) + pow_sum(cb4 - sb4, cg4 - sg4, cr4 - sr4);
					//			cout << "sum:" << sum << endl;

					if (min > sum)
					{
						min = sum;
						x = i;
						y = j;
					}
				}
			}
		}

	}

/*	for (int i = y1; i < y2; i++)
	{
		for (int j = x1; j < x2; j++)
		{
			int sb1 = src.at<Vec3b>(i, j)[0];
			int sg1 = src.at<Vec3b>(i, j)[1];
			int sr1 = src.at<Vec3b>(i, j)[2];
			int sb2 = src.at<Vec3b>(i, j + 1)[0];
			int sg2 = src.at<Vec3b>(i, j + 1)[1];
			int sr2 = src.at<Vec3b>(i, j + 1)[2];
			int sb3 = src.at<Vec3b>(i, j + 2)[0];
			int sg3 = src.at<Vec3b>(i, j + 2)[1];
			int sr3 = src.at<Vec3b>(i, j + 2)[2];
			int sb4 = src.at<Vec3b>(i + 1, j)[0];
			int sg4 = src.at<Vec3b>(i + 1, j)[1];
			int sr4 = src.at<Vec3b>(i + 1, j)[2];

			sum = pow_sum(cb1 - sb1, cg1 - sg1, cr1 - sr1) + pow_sum(cb2 - sb2, cg2 - sg2, cr2 - sr2)
				+ pow_sum(cb3 - sb3, cg3 - sg3, cr3 - sr3) + pow_sum(cb4 - sb4, cg4 - sg4, cr4 - sr4);
					//			cout << "sum:" << sum << endl;

			if (min > sum)
			{
				min = sum;
				x = i;
				y = j;
			}
		}
	}


	c.at<Vec3b>(1, 1)[0] = src.at<Vec3b>(x + 1, y + 1)[0];
	c.at<Vec3b>(1, 1)[1] = src.at<Vec3b>(x + 1, y + 1)[1];
	c.at<Vec3b>(1, 1)[2] = src.at<Vec3b>(x + 1, y + 1)[2];

	return c;
}

*/