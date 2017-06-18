#include <vector>
#include <iostream>
#include <algorithm>
#include "GraphStructure.h"

using namespace std;

//To visualize the anchor point
//Note:This function is for debugging only.
void DrawAnchorPoints(Mat input, vector<Anchor> sample_anchors, vector<Anchor> unknown_anchors,vector<Point2i> points) {
	int i = 0;
	Point2i pos;
	for (;i < unknown_anchors.size();i++) {
		pos = points[unknown_anchors[i].anchor_point];
		if (unknown_anchors[i].type == BORDER) {
			cout << "border:" << pos << endl;
			int left_x = pos.x - (PatchSizeRow - 1) / 2;
			int top_y = pos.y - (PatchSizeCol - 1) / 2;
			Rect rect(left_x, top_y, PatchSizeRow, PatchSizeCol);
			circle(input, pos, 3, Scalar(0,255,0), -1);
			//for (int j = unknown_anchors[i].begin_point;j <= unknown_anchors[i].end_point;j++) {
			//	circle(input, points[j], 1, Scalar(255, 255, 255), -1);
			//}
			rectangle(input, rect, Scalar(0, 0, 0), 1);
		}			
		else
			circle(input, pos, 1, Scalar(0, 0, 255), -1);
	}
	for (i = 0;i < sample_anchors.size();i++) {
		pos = points[sample_anchors[i].anchor_point];
		//cout << "sample index:" << sample_anchors[i].anchor_point << endl;
		//cout << "sample:" << pos << endl;
		int left_x = pos.x - (PatchSizeRow - 1) / 2;
		int top_y = pos.y - (PatchSizeCol - 1) / 2;
		Rect rect(left_x, top_y, PatchSizeRow, PatchSizeCol);
		circle(input, pos, 1, Scalar(0, 255, 255), -1);
		//rectangle(input, rect, Scalar(0, 255, 255), 1);
	}
		
	imshow("Anchor points", input);
	waitKey(0);
}

int main(){
    Mat input = imread("F:/3.jpg");
	input.convertTo(input, CV_8UC3);
    GraphStructure gs(input);
	//Rect rec(0, input.rows / 2,  input.cols, input.rows / 2);
	//Mat image = input(rec);
	//imshow("image", image);
	//waitKey(0);
    gs.getMask();
    gs.getAnchors();
	//DrawAnchorPoints(gs.image_with_mask, gs.sample_anchors[0],gs.unknown_anchors[0], gs.points[0]);
	//DrawAnchorPoints(gs.image_with_mask, gs.sample_anchors[1], gs.unknown_anchors[1], gs.points[1]);
	//cout<<"final:"<<gs.computeE2(gs.unknown_anchors[0], gs.unknown_anchors[1], gs.sample_anchors[0], gs.sample_anchors[2])<<endl;
	gs.DrawNewStructure();
	getchar();//for debug
	return 0;
}
