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
	//record the points's range contained in the patch whose center is the anchor.
	int begin_point;
	int end_point;

    double computeES();
    double computeEI();
public:
    int anchor_point;
    PointType type;
    double E1;
    vector<int> neighbors;
    vector<double> E2;//the number corresponding to the neighors.
    Anchor(int begin,int anchor,PointType t):begin_point(begin), anchor_point(anchor),type(t){
    }
    ~Anchor(){
    }
    void getNeighbors();
    void computeE1();
    void computeE2();
};
#endif // ANCHOR_H
