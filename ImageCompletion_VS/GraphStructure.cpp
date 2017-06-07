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
	int right_x = this->points[anchor].x + (PatchSizeRow - 1) / 2;
	int up_y = this->points[anchor].y - (PatchSizeCol - 1) / 2;
	int down_y = this->points[anchor].y + (PatchSizeCol - 1) / 2;
	int i = last_anchor ;
//	int threshold = 0;
	if (this->mask.at<uchar>(this->points[i]) == 0)
		type = INNER;
	else
		type = OUTER;
	while (this->points[i].x>=left_x&&this->points[i].x<=right_x&&this->points[i].y>=up_y&&this->points[i].y<=down_y) {
		if (this->mask.at<uchar>(points[i]) == 0 && type == OUTER ||
			this->mask.at<uchar>(points[i]) > 0 && type == INNER) {
				//threshold++;
				////only when the number of the points in the other area more than the threshold can the patch type BORDER
				//if(threshold>2)
					type = BORDER;
		}
			
		i++;
	}
	if (i < anchor)
		return anchor;
	return i;
}

void GraphStructure::getAnchors() {
	getCurve();
	PointType type;
	int last_anchor_point = 0;
	int anchor_point = getPointsInPatch(0, 0, type);
	int tmp;
	while (anchor_point < this->points.size()) {
		Anchor anchor(last_anchor_point, anchor_point, type);
		this->anchors.push_back(anchor);
		tmp = last_anchor_point;
		last_anchor_point = anchor_point;
		anchor_point=getPointsInPatch(tmp, anchor_point, type);
//		cout << anchor_point << endl;
	}
	cout << "anchor num:" << this->anchors.size() << endl;
	
}

float computeDist(vector<Point2i> points1, vector<Point2i> points2) {
	float result = 0.0;
	int i,j;
	float shortest;
	float tmp;
	for (i = 0;i < points1.size();i++) {
		shortest = INFINITY;
		for (j = 0;j < points2.size();j++) {
			//compute the eucliadian distance between two points.
			tmp = norm(points1[i] - points2[j]);
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

	//变换到同一尺度空间
	Point2i unknown_left_up, sample_left_up;
	unknown_left_up = leftUpPoint(unknown);
	sample_left_up = leftUpPoint(sample);
	vector<Point2i> points1;
	vector<Point2i> points2;
	for (int i = unknown.begin_point;i <= unknown.end_point;i++) {
		points1.push_back(this->points[i] - unknown_left_up);
	}
	for (int i = sample.begin_point;i <= sample.end_point;i++) {
		points2.push_back(this->points[i] - sample_left_up);
	}
	result = computeDist(points1, points2) / points_num;
	return result;
}

//Given a anchor, return the left up point of the certain patch.
Point2i GraphStructure::leftUpPoint(Anchor anchor) {
	Point2i pos;
	int index = anchor.anchor_point;
	pos.x= this->points[index].x - (PatchSizeRow - 1) / 2;
	pos.y= this->points[index].y - (PatchSizeCol - 1) / 2;
	return pos;
}

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
	int up_y = this->points[index].y - (PatchSizeCol - 1) / 2;
	Rect rect(left_x, up_y, PatchSizeRow, PatchSizeCol);
	this->image(rect).copyTo(patch);
	return patch;
}

//unknown:the anchor in the unknown area. sample: the anchor in the known area.
float GraphStructure::computeEI(Anchor unknown,Anchor sample) {
	if (unknown.type != BORDER)
		return 0.0;
	Mat patch_image;
	Mat patch_mask;
	Point2i left_up_pos = leftUpPoint(unknown);
	Rect rect(left_up_pos.x, left_up_pos.y, PatchSizeRow, PatchSizeCol);
	this->image(rect).copyTo(patch_image);
	this->mask(rect).copyTo(patch_mask);
	//a little weird, but only when the dest image and the source image are different can mask work.
	//See this link:http://code.opencv.org/issues/852
	Mat patch_im,sample_im;
	patch_image.copyTo(patch_im, patch_mask);
	Mat sample_image = getPatch(sample);
	sample_image.copyTo(sample_im, patch_mask);

	return computeSSD(patch_im, sample_im);
}


float GraphStructure::computeE1(Anchor unknown, Anchor sample) {
	return ks*computeES(unknown, sample) + ki*computeEI(unknown, sample);
}

float GraphStructure::computeE2(Anchor anchor1, Anchor anchor2,Point2i pos1,Point2i pos2)
{
	//need to compute the overlapped area first;
	Point2i rect_pos_up((pos2.x - (PatchSizeRow - 1) / 2), (pos2.y - (PatchSizeCol - 1) / 2));
	Point2i rect_pos_down((pos1.x + (PatchSizeRow - 1) / 2), (pos1.y + (PatchSizeCol - 1) / 2));
	//if there is no overlapped area.
	if (rect_pos_up.x < rect_pos_down.x || rect_pos_up.y << rect_pos_down.y) {
		cout << "Anchor" << anchor1.anchor_point << " and Anchor" << anchor2.anchor_point << "don't have overlapped area!" << endl;
		return 0.0;
	}
	//rec is the overlapped area.
	Rect rec(rect_pos_up, rect_pos_down);
	Mat pat1 = getPatch(anchor1);
	Mat pat2 = getPatch(anchor2);
	Mat patch1, patch2;
	pat1(rec).copyTo(patch1);
	pat2(rec).copyTo(patch2);
	return computeSSD(patch1,patch2);
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
