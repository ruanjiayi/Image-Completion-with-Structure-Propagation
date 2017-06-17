#include "Photometric.h"
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <iostream>
#include <cmath>

#define ATD at<Vec3d>
#define ATU at<uchar>
#define M_DST 0
#define M_SRC 1
#define M_BORDER 2

int offset_t[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };
// local offset: + offset
#define L_OFFSET(i) (y+offset_t[i][0]),(x+offset_t[i][1])
// dst offset: + patch pos + offset
#define D_OFFSET(i) (y+offset_y+offset_t[i][0]),(x+offset_x+offset_t[i][1])
// mask offset: + patch pos + 1 + offset
#define M_OFFSET(i) (y+offset_y+1+offset_t[i][0]),(x+offset_x+1+offset_t[i][1])

// init
Mat Photometric::mask;
Mat Photometric::dst;
bool Photometric::mixing = false;
double Photometric::tol = 1e-12;
double Photometric::omega = 1;

// dummy constructor
Photometric::Photometric()
{
}

Photometric::~Photometric()
{
}

// init the mask for correction
// only needs calling once
void Photometric::initMask(Mat image, Mat imageMask, uchar unknown, uchar known)
{
	Mat temp;
	Rect roi = Rect(1, 1, imageMask.size().width, imageMask.size().height);
	// update tol & omega for SOR
	tol = 1e-6;
	omega = 1.8;
	// create dst mat
	dst = Mat(imageMask.size().height, imageMask.size().width, CV_64FC3);
	image.convertTo(temp, CV_64FC3);
	temp.copyTo(dst);
	// create mask, +2 is for border
	mask = Mat(imageMask.size().height + 2, imageMask.size().width + 2, CV_8U);
	// update mask, treat unknown region as border
	mask.setTo(Scalar(unknown));
	imageMask.copyTo(mask(roi));
	Mat unknown_roi = mask == unknown;
	Mat known_roi = mask == known;
	mask.setTo(Scalar(M_BORDER), unknown_roi);
	mask.setTo(Scalar(M_DST), known_roi);
	return;
}

void Photometric::setParam(double ptol, double pomega)
{
	tol = ptol;
	omega = pomega;
	return;
}

void Photometric::useMixing(bool toggle)
{
	mixing = toggle;
	return;
}

// poisson blending with SOR
// mask is the same size with src
// offset_x|y is the relative offset
// TODO: blend with Eigen
Mat Photometric::blend(Mat dstMat, Mat srcMat, Mat maskMat, int offset_x, int offset_y)
{
	double epsilon, previous_epsilon = 1e30;
	int cnt = 0, y, x;
	Mat base, src;
	dstMat.convertTo(base, CV_64FC3);
	srcMat.convertTo(src, CV_64FC3);
	Mat result = Mat(base.size().height, base.size().width, CV_64FC3);
	base.copyTo(result);
	while (1)
	{
		double dx = 0;
		double absx = 0;
		for (y = 1; y < src.size().height - 1; y++)
		{
			for (x = 1; x < src.size().width - 1; x++)
			{
				if (maskMat.ATU(y, x) == M_SRC)
				{
					Vec3d sum_fq = Vec3d(0, 0, 0);
					Vec3d sum_vpq = Vec3d(0, 0, 0);
					Vec3d sum_boundary = Vec3d(0, 0, 0);
					Vec3d new_val = Vec3d(0, 0, 0);
					for (int i = 0; i < 4; i++)
					{
						if (maskMat.ATU(L_OFFSET(i)) == M_SRC)
						{
							sum_fq += result.ATD(D_OFFSET(i));
							for (int ch = 0; ch < 3; ch++)
							{
								if (mixing&&abs(src.ATD(y, x)(ch) - src.ATD(L_OFFSET(i))(ch)) <
									abs(base.ATD(y + offset_y, x + offset_x)(ch) - base.ATD(D_OFFSET(i))(ch)))
								{
									sum_vpq(ch) += base.ATD(y + offset_y, x + offset_x)(ch) - base.ATD(D_OFFSET(i))(ch);
								}
								else
								{
									sum_vpq(ch) += src.ATD(y, x)(ch) - src.ATD(L_OFFSET(i))(ch);
								}
							}
						}
						else
						{
							sum_boundary += base.ATD(D_OFFSET(i));
						}
					}
					new_val = (sum_fq + sum_vpq + sum_boundary) / 4.0;
					dx += abs(new_val(0) - result.ATD(y + offset_y, x + offset_x)(0))
						+ abs(new_val(1) - result.ATD(y + offset_y, x + offset_x)(1))
						+ abs(new_val(2) - result.ATD(y + offset_y, x + offset_x)(2));
					absx += abs(new_val(0)) + abs(new_val(1)) + abs(new_val(2));
					result.ATD(y + offset_y, x + offset_x) = (1 - omega)*result.ATD(y + offset_y, x + offset_x) + omega*new_val;
				}
			}
		}
		cnt++;
		epsilon = dx / absx;
		if (abs(epsilon) < 1e-15 || abs(previous_epsilon - epsilon) < tol)
		{
			break;
		}
		else
		{
			previous_epsilon = epsilon;
		}
	}
	result.convertTo(result, CV_8UC3);
	std::cout << "Iteration times: " << cnt << std::endl;
	return result;
}

// in that case, mask can be arbitrary shape
Mat Photometric::blendE(Mat dstMat, Mat srcMat, Mat maskMat, int offset_x, int offset_y)
{
	Mat base, src;
	dstMat.convertTo(base, CV_64FC3);
	srcMat.convertTo(src, CV_64FC3);
	Mat result = Mat(base.size().height, base.size().width, CV_64FC3);
	base.copyTo(result);
	// get actual size of coefficient matrix
	int width, height, y, x, i, ch, cnt = 0;
	width = src.size().width;
	height = src.size().height;
	Mat index = Mat(height, width, CV_32S);
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			if (maskMat.ATU(y, x) == M_SRC)
			{
				index.at<int>(y, x) = cnt;
				cnt++;
			}
		}
	}
	// init Ax=b
	Eigen::SparseMatrix<double> A;
	Eigen::VectorXd b[3], sol[3];
	A = Eigen::SparseMatrix<double>(cnt, cnt);
	A.reserve(Eigen::VectorXd::Constant(cnt, 5));
	for (i = 0; i < 3; i++)
	{
		b[i] = Eigen::VectorXd(cnt);
		sol[i] = Eigen::VectorXd(cnt);
	}
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			if (maskMat.ATU(y, x) == M_SRC)
			{
				for (ch = 0; ch < 3; ch++)
				{
					double sum_vpq = 0, sum_boundary = 0;
					// neighbors
					double neighbor = 0;
					// traverse neighbors
					for (i = 0; i < 4; i++)
					{
						if (y + offset_t[i][0] < 0
							|| y + offset_t[i][0] >= height
							|| x + offset_t[i][1] < 0
							|| x + offset_t[i][1] >= width)
						{
							continue;
						}
						neighbor += 1.0;
						if (maskMat.ATU(L_OFFSET(i)) == M_SRC)
						{
							if (mixing&&abs(src.ATD(y, x)(ch) - src.ATD(L_OFFSET(i))(ch)) <
								abs(base.ATD(y + offset_y, x + offset_x)(ch) - base.ATD(D_OFFSET(i))(ch)))
							{
								sum_vpq += base.ATD(y + offset_y, x + offset_x)(ch) - base.ATD(D_OFFSET(i))(ch);
							}
							else
							{
								sum_vpq += src.ATD(y, x)(ch) - src.ATD(L_OFFSET(i))(ch);
							}
							if (ch == 0)
							{
								A.insert(index.at<int>(y, x), index.at<int>(L_OFFSET(i))) = -1.0;
							}
						}
						else
						{
							sum_boundary += base.ATD(D_OFFSET(i))(ch);
						}
					}
					if (ch == 0)
					{
						A.insert(index.at<int>(y, x), index.at<int>(y, x)) = neighbor;
					}
					b[ch](index.at<int>(y, x)) = sum_boundary + sum_vpq;
				}
			}
		}
	}
	// eigen solver
	Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver;
	solver.compute(A);
	if (solver.info() != Eigen::Success)
	{
		std::cout << "decomposition failed" << std::endl;
		return result;
	}
	for (ch = 0; ch < 3; ch++)
	{
		sol[ch] = solver.solve(b[ch]);
		if (solver.info() != Eigen::Success)
		{
			std::cout << "solving failed" << std::endl;
			return result;
		}
	}
	for (ch = 0; ch < 3; ch++)
	{
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				if (maskMat.ATU(y, x) == M_SRC)
				{
					result.ATD(y + offset_y, x + offset_x)(ch) = sol[ch](index.at<int>(y, x));
				}
			}
		}
	}
	result.convertTo(result, CV_8UC3);
	return result;
}

// every time you get a new patch, you need correct it
// using seamless clone, importing method with slight modification
// this mask is the same size with dst, offset_x|y is the absolute offset
// I'll turn to eigen lib for efficiency
void Photometric::correct(Mat &patch, int offset_x, int offset_y)
{
	// eps for iteration
	double epsilon, previous_epsilon = 1e30;
	// infos
	int width, height, y, x, i, cnt = 0;
	width = patch.size().width;
	height = patch.size().height;
	// src: patch with double type
	// result: the modified patch
	Mat src;
	patch.convertTo(src, CV_64FC3);
	Mat result = Mat(height, width, CV_64FC3);
	src.copyTo(result);
	// update dst mask: the blend region
	Rect patch_mask = Rect(offset_x + 1, offset_y + 1, width, height);
	mask(patch_mask).setTo(Scalar(1));
	// SOR iteration
	while (1)
	{
		// accumulated difference
		double dx = 0, absx = 0;
		// traverse all f_q in patch
		// we know that the patch is a square
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				// useless checking
				if (mask.ATU(y + offset_y + 1, x + offset_x + 1) == 1)
				{
					// neighbors
					double neighbor = 0;
					// for SOR iteration
					Vec3d sum_fq = Vec3d(0, 0, 0);
					Vec3d sum_vpq = Vec3d(0, 0, 0);
					Vec3d sum_boundary = Vec3d(0, 0, 0);
					Vec3d new_val = Vec3d(0, 0, 0);
					// traverse neighbors
					for (i = 0; i < 4; i++)
					{
						switch (mask.ATU(M_OFFSET(i)))
						{
						case M_BORDER:
							// border, truncated neighborhood
							break;
						case M_SRC:
							// in region
							sum_fq += result.ATD(L_OFFSET(i));
							// gradient
							sum_vpq += src.ATD(y, x) - src.ATD(L_OFFSET(i));
							neighbor += 1.0;
							break;
						case M_DST:
							// known region at boundary
							sum_boundary += dst.ATD(D_OFFSET(i));
							// in photometric correction, v_pq at boundary setted to 0
							// sum_vpq += src.ATD(y, x) - src.ATD(L_OFFSET(i));
							neighbor += 1.0;
							break;
						}
					}
					// compute new f_p
					new_val = (sum_fq + sum_vpq + sum_boundary) / neighbor;
					// update difference
					dx += abs(new_val(0) - result.ATD(y, x)(0))
						+ abs(new_val(1) - result.ATD(y, x)(1))
						+ abs(new_val(2) - result.ATD(y, x)(2));
					absx += abs(new_val(0)) + abs(new_val(1)) + abs(new_val(2));
					// update result
					result.ATD(y, x) = (1 - omega)*result.ATD(y, x) + omega*new_val;
				}
			}
		}
		cnt++;
		epsilon = dx / absx;
		if (fabs(epsilon) < 1e-15 || fabs(previous_epsilon - epsilon) < tol)
		{
			break;
		}
		else
		{
			previous_epsilon = epsilon;
		}
	}
	// update mask
	mask(patch_mask).setTo(Scalar(M_DST));
	// get result
	Mat uresult;
	result.convertTo(uresult, CV_8UC3);
	uresult.copyTo(patch);
	// update dst
	result.copyTo(dst(patch_mask));
	// iteration times
	std::cout << "Iteration times: " << cnt << std::endl;
	return;
}
// TODO: an unify wrapper for SOR or Eigen Direct solver
// TODO: elegant way to build the matrices
void Photometric::correctE(Mat & patch, int offset_x, int offset_y)
{
	// infos
	int width, height, y, x, i, cnt = 0;
	width = patch.size().width;
	height = patch.size().height;
	Eigen::SparseMatrix<double> A;
	Eigen::VectorXd b[3], sol[3];
	A = Eigen::SparseMatrix<double>(height*width, height*width);
	A.reserve(Eigen::VectorXd::Constant(height*width, 5));
	for (i = 0; i < 3; i++)
	{

		b[i] = Eigen::VectorXd(height*width);
		sol[i] = Eigen::VectorXd(height*width);
	}
	// src: patch with double type
	// result: the modified patch
	Mat src;
	patch.convertTo(src, CV_64FC3);
	Mat result = Mat(height, width, CV_64FC3);
	src.copyTo(result);
	// update dst mask: the blend region
	Rect patch_mask = Rect(offset_x + 1, offset_y + 1, width, height);
	mask(patch_mask).setTo(Scalar(1));
	// index
	Mat index = Mat(height, width, CV_32S);
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			index.at<int>(y, x) = y*width + x;
		}
	}
	// traverse all f_q in patch
	// we know that the patch is a square
	// may using matrix manipulations if i have enough time
	int ch;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			for (ch = 0; ch < 3; ch++)
			{
				double sum_vpq = 0, sum_boundary = 0;
				// neighbors
				double neighbor = 0;
				// traverse neighbors
				for (i = 0; i < 4; i++)
				{
					switch (mask.ATU(M_OFFSET(i)))
					{
					case M_BORDER:
						// border, truncated neighborhood
						break;
					case M_SRC:
						// in region
						if (ch == 0)
							A.insert(index.at<int>(y, x), index.at<int>(L_OFFSET(i))) = -1.0;
						// gradient
						sum_vpq += src.ATD(y, x)(ch) - src.ATD(L_OFFSET(i))(ch);
						neighbor += 1.0;
						break;
					case M_DST:
						// known region at boundary
						sum_boundary += dst.ATD(D_OFFSET(i))(ch);
						neighbor += 1.0;
						break;
					}
				}
				if (ch == 0)
					A.insert(index.at<int>(y, x), index.at<int>(y, x)) = neighbor;
				b[ch](index.at<int>(y, x)) = sum_boundary + sum_vpq;
			}
		}
	}
	Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver;
	solver.compute(A);
	if (solver.info() != Eigen::Success)
	{
		std::cout << "decomposition failed" << std::endl;
		return;
	}
	for (ch = 0; ch < 3; ch++)
	{
		sol[ch] = solver.solve(b[ch]);
		if (solver.info() != Eigen::Success)
		{
			std::cout << "solving failed" << std::endl;
			return;
		}
	}
	for (ch = 0; ch < 3; ch++)
	{
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				result.ATD(y, x)(ch) = sol[ch](index.at<int>(y, x));
			}
		}
	}
	// update mask
	mask(patch_mask).setTo(Scalar(M_DST));
	// get result
	Mat uresult;
	result.convertTo(uresult, CV_8UC3);
	uresult.copyTo(patch);
	// update dst
	result.copyTo(dst(patch_mask));
	return;
}