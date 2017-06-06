#include <string>
#include <vector>
#include <iostream>
#include "GraphStructure.h"

using namespace std;

//To visualize the anchor point
//Note:This function is for debugging only.
void DrawAnchorPoints(Mat input, vector<Anchor> anchors, vector<Point2i> points) {
	int i = 0;
	Point2i pos;
	for (;i < anchors.size();i++) {
		pos = points[anchors[i].anchor_point];
		if(anchors[i].type==INNER)
			circle(input, pos, 1, Scalar(255, 0, 0), -1);
		else if (anchors[i].type == OUTER)
			circle(input, pos, 1, Scalar(0, 255, 0), -1);
		else
			circle(input, pos, 1, Scalar(0, 0, 255), -1);

	}
	imshow("Anchor points", input);
	waitKey(0);
}

int main(){
    Mat input = imread("F:/lena.png");
	input.convertTo(input, CV_8UC3);
    GraphStructure gs(input);
    gs.getMask();
    gs.getAnchors();
	//DrawAnchorPoints(input, gs.anchors, gs.points);

	getchar();//for debug
	return 0;
}
