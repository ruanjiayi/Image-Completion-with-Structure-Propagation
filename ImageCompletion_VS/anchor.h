#ifndef ANCHOR_H
#define ANCHOR_H

#include <opencv2\opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

#define ks 50
#define ki 2
#define PatchSizeRow 19
#define PatchSizeCol 19

typedef enum{
    BORDER,
    INNER,
    OUTER
}PointType;

//contain all the information of a patch.
class Anchor{
private:
	bool converged;
	
public:
	//record the points's range contained in the patch whose center is the anchor.
	int begin_point;
	int end_point;
    int anchor_point;
    PointType type;
	//vector<Anchor> neighbors;
	vector<int> neighbors;//the index of the neighbors in unknown_anchor vector.
	Anchor(int begin, int anchor, int end, PointType t);
	~Anchor();
	bool if_converged();
	void set_converge();
};
#endif // ANCHOR_H
