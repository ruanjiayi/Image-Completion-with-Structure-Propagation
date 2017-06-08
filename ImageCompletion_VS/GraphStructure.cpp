#include "GraphStructure.h"

#include <iostream>
using namespace std;

static Point2i mouse_pos;
static int mouse_event;
static int mouse_flags;

static void mouseCallBack(int event, int x, int y, int flags, void* ustc)
{
	mouse_event = event;
	mouse_pos = Point(x, y);
	mouse_flags = flags;
}

GraphStructure::GraphStructure(Mat input):image(input){
    this->mask=Mat::ones(input.size(),CV_8U);
	this->mask.setTo(255);
    image_with_mask=image.clone();
}

//Get the user's specified unknown area.
void GraphStructure::getMask()
{
    Mat toshow = this->image.clone();
    int size = 30;
	imshow("Generate Input Mask", toshow);
	setMouseCallback("Generate Input Mask", mouseCallBack);
    Point2i last_pos(-1,-1);
	while (1)
	{
		Mat totoshow = toshow.clone();
		char key = waitKey(10);
		if (key == '[')
		{
			size = (size > 1) ? size - 1 : size;//let the brush size be smaller
		}
		else if (key == ']')
		{
			size++;//let the brush size be bigger
		}
		if (mouse_event == CV_EVENT_RBUTTONDOWN)
		{
			break;
		}
		if ((mouse_event == CV_EVENT_MOUSEMOVE && (mouse_flags & CV_EVENT_FLAG_LBUTTON)) ||
				(mouse_event == CV_EVENT_LBUTTONDOWN))
		{
//            circle(this->mask, mouse_pos, size, Scalar(255), -1);
//            circle(this->image_with_mask, mouse_pos, size, Scalar(255), -1);
//			circle(toshow, mouse_pos, size, Scalar(255, 0, 0), -1);
//			circle(totoshow, mouse_pos, size, Scalar(255, 0, 0), -1);
            if(last_pos.x != -1 && last_pos.y != -1){
                line(this->mask, last_pos, mouse_pos, Scalar(0),1.5*size);
                line(toshow, last_pos, mouse_pos, Scalar(255, 0, 0),1.5*size);
                line(this->image_with_mask, last_pos, mouse_pos, Scalar(255, 0, 0),1.5*size);
            }
            last_pos=mouse_pos;
		}
        else{
            last_pos.x=-1;
            last_pos.y=-1;
        }
		circle(totoshow, mouse_pos, size, Scalar(255, 0, 0), -1);
		imshow("Generate Input Mask", totoshow);
	}
	destroyWindow("Generate Input Mask");
    return;
}

//Get the user's specified curve.
void GraphStructure::getCurve(){
    Mat toshow = this->image_with_mask.clone();
    int size = 5;
    imshow("Generate Specified Curves", toshow);
    setMouseCallback("Generate Specified Curves", mouseCallBack);
    Point2i last_pos(-1,-1);
    Point2i last_point(-1,-1);
    while (1)
    {
        Mat totoshow = toshow.clone();
        if(waitKey(10)==27)
           break;
        else if ((mouse_event == CV_EVENT_MOUSEMOVE && (mouse_flags == CV_EVENT_FLAG_LBUTTON)) ||
                (mouse_event == CV_EVENT_LBUTTONDOWN))
        {
            if(last_pos!=Point2i(-1,-1)){
                line(toshow, last_pos, mouse_pos, Scalar(0, 255, 255),size*1.5);
				//the LintIterator is used to get the every point on the line.
                LineIterator it(toshow, last_pos, mouse_pos, 8);
                //cout<<it.count<<endl;
               // cout<<mouse_pos.x-last_pos.x<<" "<<mouse_pos.y-last_pos.y<<endl;
                for(int i = 0; i < it.count; i++, ++it){
                    if(last_point==Point2i(-1,-1)||last_point!=it.pos()){
                        this->points.push_back(it.pos());
                        last_point=it.pos();
                        //cout<<it.pos().x<<" "<<it.pos().y<<endl;
                    }
                }
            }
            last_pos=mouse_pos;
        }
        else{
            last_pos=Point2i(-1,-1);
        }
        circle(totoshow, mouse_pos, size, Scalar(0, 255, 255), -1);
        imshow("Generate Specified Curves", totoshow);
    }
    cout<<"points num:"<<this->points.size()<<endl;
    destroyWindow("Generate Specified Curves");
}

//Get the patch type and the points' last index
//(as the next anchor to ensure that the interval length between two anchors is half of the patch size.)
int  GraphStructure::getPointsInPatch(int last_anchor, int anchor,PointType& type) {
	int left_x = this->points[anchor].x - (PatchSizeRow - 1) / 2;
	//int right_x = this->points[anchor].x + (PatchSizeRow - 1) / 2;
	int top_y = this->points[anchor].y - (PatchSizeCol - 1) / 2;
	//int bottom_y = this->points[anchor].y + (PatchSizeCol - 1) / 2;
	int i = last_anchor  ;
	Rect rec(left_x, top_y, PatchSizeRow, PatchSizeCol);
	//find the first point to enter the patch area.
	while (!rec.contains(this->points[i]))
	{
		i++;
	}
	if (this->mask.at<uchar>(this->points[i]) == 0)
		type = INNER;
	else
		type = OUTER;
	while(rec.contains(this->points[i])
		/*this->points[i].x>=left_x&&this->points[i].x<=right_x&&this->points[i].y>=top_y&&this->points[i].y<=bottom_y*/) {
		if ((this->mask.at<uchar>(points[i]) == 0 && type == OUTER )||
			(this->mask.at<uchar>(points[i]) > 0 && type == INNER)) {
				//cout << (int)this->mask.at<uchar>(points[i]) << endl;
				type = BORDER;
		}
		i++;
	}
	if (i < anchor) {
		cout << "getPatch() exception" << endl;
		return anchor;
	}
	return i;
}

void GraphStructure::pushIntoAnchors(Anchor anchor) {
	if (anchor.type == OUTER)
		this->sample_anchors.push_back(anchor);
	else
		this->unknown_anchors.push_back(anchor);
}

void GraphStructure::getAnchors() {
	getCurve();
	PointType type;
	int last_anchor_point = 0;
	int anchor_point = getPointsInPatch(last_anchor_point, 0, type);
	int next_anchor_point = getPointsInPatch(last_anchor_point,anchor_point, type);
	while (next_anchor_point < this->points.size()) {
		Anchor anchor(last_anchor_point, anchor_point, next_anchor_point - 1, type);
		pushIntoAnchors(anchor);
		last_anchor_point = anchor_point;
		anchor_point = next_anchor_point;
		next_anchor_point = getPointsInPatch(last_anchor_point, anchor_point, type);
//		cout << anchor_point << endl;
	}
	cout << "sample anchor num:" << this->sample_anchors.size() << endl;
	cout << "unknown anchor num:" << this->unknown_anchors.size() << endl;
}

float computeDist(vector<Point2i> points1, vector<Point2i> points2) {
	float result = 0.0;
	int i,j;
	float shortest;
	float tmp;
	float normalized = (float)norm(Point2i(PatchSizeRow, PatchSizeCol));

	for (i = 0;i < points1.size();i++) {
		shortest = INFINITY;
		for (j = 0;j < points2.size();j++) {
			//compute the eucliadian distance between two points.
			tmp = (float)norm(points1[i] - points2[j]) / normalized;
			tmp *= tmp;
			if (tmp < shortest)
				shortest = tmp;
		}
		result += shortest;
	}

	return result;
}

float GraphStructure::computeES(Anchor unknown,Anchor sample) {
	float result;
	int points_num = unknown.end_point - unknown.begin_point + 1;

	//Transform these two patches into the same scale space.
	Point2i unknown_left_top, sample_left_top;
	unknown_left_top = leftTopPoint(unknown);
	sample_left_top = leftTopPoint(sample);
	vector<Point2i> points1;
	vector<Point2i> points2;
	for (int i = unknown.begin_point;i <= unknown.end_point;i++) {
		points1.push_back(this->points[i] - unknown_left_top);
	}
	for (int i = sample.begin_point;i <= sample.end_point;i++) {
		points2.push_back(this->points[i] - sample_left_top);
	}
	result = computeDist(points1, points2);
	result += computeDist(points2, points1);
	result /= points_num;// ES(xi) is further normalized by dividing the total number of points in ci
	//cout << "ES:" << result << endl;
	return result;
}

//Given a anchor, return the left top point of the certain patch.
Point2i GraphStructure::leftTopPoint(Anchor anchor) {
	Point2i pos;
	int index = anchor.anchor_point;
	pos.x= this->points[index].x - (PatchSizeRow - 1) / 2;
	pos.y= this->points[index].y - (PatchSizeCol - 1) / 2;
	return pos;
}

//Point2i GraphStructure::RightbottomPoint(Anchor anchor) {
//	Point2i pos;
//	int index = anchor.anchor_point;
//	pos.x = this->points[index].x + (PatchSizeRow - 1) / 2;
//	pos.y = this->points[index].y + (PatchSizeCol - 1) / 2;
//	return pos;
//}

//Given two images, compute the sum of the normalized squared diﬀerences between them.
float computeSSD(Mat m1, Mat m2) {
	Mat result;
	matchTemplate(m1, m2, result, CV_TM_SQDIFF_NORMED);
	return result.at<float>(0, 0);
}

//Get the patch image(type:mat) of a given anchor.
Mat GraphStructure::getPatch(Anchor anchor) {
	Mat patch;
	int index = anchor.anchor_point;
	int left_x = this->points[index].x - (PatchSizeRow - 1) / 2;
	int top_y = this->points[index].y - (PatchSizeCol - 1) / 2;
	Rect rect(left_x, top_y, PatchSizeRow, PatchSizeCol);
	this->image(rect).copyTo(patch);
	return patch;
}

//unknown:the anchor in the unknown area. sample: the anchor in the known area.
float GraphStructure::computeEI(Anchor unknown,Anchor sample) {
	if (unknown.type != BORDER)
		return 0.0;
	Mat patch_image;
	Mat patch_mask;
	Point2i left_top_pos = leftTopPoint(unknown);
	Rect rect(left_top_pos.x, left_top_pos.y, PatchSizeRow, PatchSizeCol);
	this->image(rect).copyTo(patch_image);
	this->mask(rect).copyTo(patch_mask);
	//a little weird, but only when the dest image and the source image are different can mask work.
	//See this link:http://code.opencv.org/issues/852
	Mat patch_im,sample_im;
	patch_image.copyTo(patch_im, patch_mask);
	Mat sample_image = getPatch(sample);
	sample_image.copyTo(sample_im, patch_mask);

	float result = computeSSD(patch_im, sample_im);
	//cout << "EI:"<< result << endl;
	return result;
}

float GraphStructure::computeE1(Anchor unknown, Anchor sample) {
	return ks*computeES(unknown, sample) + ki*computeEI(unknown, sample);
}

float GraphStructure::computeE2(Anchor unknown1, Anchor unknown2, Anchor sample1, Anchor sample2)
{
	//need to compute the overlapped area first;
	//The four corner pointer of the two patch is named after ABCDEFGH in order.(from left to right,from top to bottom)
	Point2i A = leftTopPoint(unknown1);
	Point2i E = leftTopPoint(unknown2);

	Rect rec1(A.x, A.y, PatchSizeRow, PatchSizeCol);
	Rect rec2(E.x, E.y, PatchSizeRow, PatchSizeCol);
	Rect rec_intersect = rec1&rec2;
	Mat pat1 = getPatch(sample1);
	Mat pat2 = getPatch(sample2);
	//This submatrix(large(rec)) will be a REFERENCE to PART of full matrix, NOT a copy
	Mat large1(this->image.size(),this->image.type(),Scalar(0,0,0));
	Mat large2(this->image.size(), this->image.type(), Scalar(0, 0, 0));

	pat1.copyTo(large1(rec1));
	pat2.copyTo(large2(rec2));
	Mat patch1, patch2;
	large1(rec_intersect).copyTo(patch1);
	large2(rec_intersect).copyTo(patch2);

	//cout << computeSSD(large1, large2)<<endl;
	//cout << computeSSD(pat1, pat2)<<endl;

	//cout << patch1.size() << endl;
	//cout << patch2.size() << endl;
	//rectangle(large1, rec_intersect, Scalar(255, 255, 255));
	//rectangle(large2, rec_intersect, Scalar(255, 255, 255));
	//imshow("patch1", large1);
	//imshow("patch2", large2);
	//waitKey(0);

	//cout << "computeE2: " << computeSSD(patch1, patch2) << endl;

	return computeSSD(patch1,patch2);
}

//To get the best label for every unknown anchor with DP method.
vector<int> GraphStructure::DP() {
	vector<int> sample_index;
	int sample_size = this->sample_anchors.size();
	int unknown_size = this->unknown_anchors.size();
	float **M = new float*[unknown_size];
	int **index = new int*[unknown_size];
	for (int i = 0;i < unknown_size;i++) {
		M[i] = new float[sample_size];
		index[i] = new int[sample_size];
	}
		
	for (int i = 0;i < sample_size;i++) {
		M[0][i] = computeE1(this->unknown_anchors[0], this->sample_anchors[i]);
	}

	int i, j, k;
	float E1;
	float min_tmp=INFINITY;
	float tmp;
	int index_tmp;
	for ( i = 1;i < unknown_size;i++) {
		for (j = 0;j < sample_size;j++) {
			min_tmp = INFINITY;
			E1 = computeE1(this->unknown_anchors[i], this->sample_anchors[j]);
			for (k = 0;k < sample_size;k++) {
				tmp = M[i - 1][k]+ computeE2(this->unknown_anchors[i - 1],
					this->unknown_anchors[i], this->sample_anchors[k], this->sample_anchors[j]);
				if (tmp < min_tmp) {
					min_tmp = tmp;
					index_tmp = k;
				}
					
			}
			M[i][j] = E1 + min_tmp;
			index[i][j] = index_tmp;
			//cout << ".";
		}
	}

	min_tmp = INFINITY;
	for (i = 0;i < sample_size;i++) {
		tmp = M[unknown_size - 1][i];
		if (tmp < min_tmp) {
			min_tmp = tmp;
			index_tmp = i;
		}
	}

	cout << "The min energy is:" << min_tmp << endl;
	sample_index.push_back(index_tmp);
	for (i = unknown_size - 1;i > 0;i--) {
		index_tmp = index[i][index_tmp];
		sample_index.push_back(index_tmp);
	}
	reverse(sample_index.begin(), sample_index.end());
	cout << "sample_index_size:" << sample_index.size() << endl;
	for (int i = 0;i < sample_index.size();i++)
		cout << sample_index[i] << endl;

	return sample_index;
}

void GraphStructure::copyToLargePic(Anchor unknown, Mat patch, Mat large) {
	Point2i left_top = leftTopPoint(unknown);
	Rect rec(left_top.x, left_top.y, PatchSizeRow, PatchSizeCol);
	//This submatrix(large(rec)) will be a REFERENCE to PART of full matrix, NOT a copy
	patch.copyTo(large(rec));
}

void GraphStructure::DrawNewStructure() {
	vector<int> sample_index = DP();
	Mat background = this->image_with_mask;
	int index;
	for (int i = 0;i < sample_index.size();i++) {
		index = sample_index[i];
		Mat patch = getPatch(this->sample_anchors[index]);
		copyToLargePic(this->unknown_anchors[i], patch, background);
	}

	imshow("new pic", background);
	waitKey(0);
}

//Mat getRegions(Mat input, Mat mask, vector<icPoint> points)
//{
//	int top = input.rows, bottom = 0;
//	int left = input.cols, right = 0;
//	for (int i = 0; i < points.size(); i++)
//	{
//		if (points[i].pos.x < left) left = points[i].pos.x;
//		if (points[i].pos.x > right) right = points[i].pos.x;
//		if (points[i].pos.y < top) top = points[i].pos.y;
//		if (points[i].pos.y > bottom) bottom = points[i].pos.y;
//	}
//	return Mat();
//}
