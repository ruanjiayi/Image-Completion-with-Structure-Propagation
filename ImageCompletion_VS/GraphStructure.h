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
	//Get the user's specified curve
	//and record these consecutive points in the vector points.
	void getCurve();

	//Given the last_anchor index as the first one to begin in the patch
	//return the point of the last index in the patch and the type of the patch.
	int  getPointsInPatch(int last_anchor, int anchor, PointType& type);
	Mat GraphStructure::getPatch(Anchor anchor);
	Point2i leftUpPoint(Anchor anchor);
public:
	Mat image_with_mask;
    GraphStructure(Mat input);
    ~GraphStructure(){
    }
	vector<Anchor> anchors;
	vector<Point2i> points;
	//Get the user's specified unknown area.
    void getMask();

    // Get the sparse sample points on the curve as the anchor points 
	//and record the result in the vector anchors.
    void getAnchors();

    // Ask the user to specify the numbers of regions
    void getRegions();

	float computeEI(Anchor unknown, Anchor sample);
	float computeE1(Anchor unknown, Anchor sample);
	float computeE2(Anchor anchor1, Anchor anchor2, Point2i pos1, Point2i pos2);
	float computeES(Anchor unknown, Anchor sample);
};

#endif
