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

	//check the type and decide which vector(unknown_anchors or sample_anchors)to push into
	void pushIntoAnchors(Anchor anchor);
	
	//Given a anchor, return the left top point of the certain patch.
	Point2i leftTopPoint(Anchor anchor);

	//Given unknown anchor point, copy the patch into the position of the point in the large picture.
	void copyToLargePic(Anchor unknown, Mat patch, Mat large);

	//Get the patch image(type:mat) of a given anchor.
	Mat GraphStructure::getPatch(Anchor anchor);

	//unknown:the anchor in the unknown area. sample: the anchor in the known area.
	float computeEI(Anchor unknown, Anchor sample);
	float computeES(Anchor unknown, Anchor sample);
	float computeE1(Anchor unknown, Anchor sample);
	float computeE2(Anchor unknown1, Anchor unknown2, Anchor sample1, Anchor sample2);

	//To get the best label for every unknown anchor with DP method.
	vector<int> DP();
public:
	Mat image_with_mask;
    GraphStructure(Mat input);
    ~GraphStructure(){
    }
	vector<Anchor> sample_anchors;
	vector<Anchor> unknown_anchors;
	vector<Point2i> points;

	//Get the user's specified unknown area.
    void getMask();

    //Get the sparse sample points on the curve as the anchor points 
	//and record the result in the vector anchors.
    void getAnchors();

    //Ask the user to specify the numbers of regions
    void getRegions();

	//Draw the new picture with the propagated structure
	void DrawNewStructure();
};

#endif
