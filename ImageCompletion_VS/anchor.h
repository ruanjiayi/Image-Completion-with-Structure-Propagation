#ifndef ANCHOR_H
#define ANCHOR_H

#include <opencv2\opencv.hpp>
using namespace cv;

#define ks 50
#define ki 2
#define PatchSizeRow 9
#define PatchSizeCol 9

typedef enum{
    BORDER,
    INNER,
    OUTER
}PointType;

//contain all the information of a patch.
class Anchor{
private:

public:
	//record the points's range contained in the patch whose center is the anchor.
	int begin_point;
	int end_point;
    int anchor_point;
    PointType type;
    vector<Anchor> neighbors;
	Anchor(int begin, int anchor, int end, PointType t);
	Anchor() {
	}
    ~Anchor(){
    }
};
#endif // ANCHOR_H
