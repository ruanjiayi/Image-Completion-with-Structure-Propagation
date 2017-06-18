#ifndef _GRAPH_STRUCTURE_H
#define _GRAPH_STRUCTURE_H

#include <opencv2\opencv.hpp>
#include "anchor.h"
using namespace cv;

class GraphStructure{
private:
    Mat image;
    Mat mask;
    Mat regions;

	//Given the last_anchor index as the first one to begin in the patch
	//return the point of the last index in the patch and the type of the patch.
	int  getPointsInPatch(int last_anchor, int anchor, PointType& type, int point_index);

	//check the type and decide which vector(unknown_anchors or sample_anchors)to push into
	//void pushIntoAnchors(Anchor anchor);
	
	//Given a anchor, return the left top point of the certain patch.
	Point2i leftTopPoint(Anchor anchor, int point_index);

	//Given unknown anchor point, copy the patch into the position of the point in the large picture.
	void copyToLargePic(Anchor unknown, Mat patch, Mat large, int point_index);

	bool ifLineIntersect(int i1, int i2);

	//Get the patch image(type:mat) of a given anchor.
	Mat GraphStructure::getPatch(Anchor anchor,  int point_index);

	//unknown:the anchor in the unknown area. sample: the anchor in the known area.
	float computeEI(Anchor unknown, Anchor sample,int point_index);
	float computeES(Anchor unknown, Anchor sample, int point_index);
	float computeE1(Anchor unknown, Anchor sample, int point_index);
	float computeE2(Anchor unknown1, Anchor unknown2, Anchor sample1, Anchor sample2, int point_index);

	//To get the best label for every unknown anchor with DP method.
	vector<int> DP(vector<Anchor> sample, vector<Anchor> unknown, int point_index);

	void GraphStructure::DrawOneLine(vector<Anchor> sample, vector<Anchor> unknown, int point_index,bool flag);

	void GraphStructure::computeAnchors(int point_index, vector<Anchor>& sample, vector<Anchor>& unknown);

public:
	Mat image_with_mask;
    GraphStructure(Mat input);
    ~GraphStructure(){
    }
	vector< vector<Anchor> > sample_anchors;
	vector< vector<Anchor> > unknown_anchors;
	vector< vector<Point2i> > points;

	//Get the user's specified unknown area.
    void getMask();

    //Get the sparse sample points on the curve as the anchor points 
	//and record the result in the vector anchors.
    void getAnchors();

	//Get the user's specified curve
	//and record these consecutive points in the vector points.
	void getCurve();

    //Ask the user to specify the numbers of regions
    void getRegions();

	vector<int> BP(vector<Anchor> sample, vector<Anchor> unknown, int point_index);

	//Draw the new picture with the propagated structure
	void DrawNewStructure();

};

#endif
