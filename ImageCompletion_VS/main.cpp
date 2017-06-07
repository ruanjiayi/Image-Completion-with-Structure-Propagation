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
		if (anchors[i].type == BORDER) {
			int left_x = pos.x - (PatchSizeRow - 1) / 2;
			int up_y = pos.y - (PatchSizeCol - 1) / 2;
			Rect rect(left_x, up_y, PatchSizeRow, PatchSizeCol);
			circle(input, pos, 1, Scalar(0, 0, 0), -1);
			rectangle(input, rect, Scalar(0, 0, 0), 1);
		}			
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
	DrawAnchorPoints(gs.image_with_mask, gs.anchors, gs.points);
	
	getchar();//for debug
	return 0;
}
