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
    mask=Mat::zeros(input.size(),CV_8U);
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
                line(this->mask, last_pos, mouse_pos, Scalar(255, 0, 0),1.5*size);
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


//Get the user's specified curve
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

//Get the patch type and the points' endding index.
int  GraphStructure::getPointsInPatch(int last_anchor, int anchor,PointType& type) {
	int left_x = this->points[anchor].x - (PatchSizeRow - 1) / 2;
	int right_x = this->points[anchor].x + (PatchSizeRow - 1) / 2;
	int up_y = this->points[anchor].y - (PatchSizeCol - 1) / 2;
	int down_y = this->points[anchor].y + (PatchSizeCol - 1) / 2;
	int i = last_anchor + 1;
	if (this->mask.at<uchar>(this->points[i]) > 0)
		type = INNER;
	else
		type = OUTER;
	while (this->points[i].x>left_x&&this->points[i].x<right_x&&this->points[i].y>up_y&&this->points[i].y<down_y) {
		if (this->mask.at<uchar>(points[i]) > 0 && type == OUTER||
			this->mask.at<uchar>(points[i]) == 0 && type == INNER)
			type = BORDER;
		i++;
	}
	return i;
}


void GraphStructure::getAnchors() {
	getCurve();
	PointType type;
	int last_anchor_point = 0;
	int anchor_point = getPointsInPatch(-1, 0, type);
	int tmp;
	while (anchor_point < this->points.size()) {
		Anchor anchor(last_anchor_point, anchor_point, type);
		this->anchors.push_back(anchor);
		tmp = last_anchor_point;
		last_anchor_point = anchor_point;
		anchor_point=getPointsInPatch(tmp, anchor_point, type);
	}
	cout << "anchor num:" << this->anchors.size() << endl;
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
