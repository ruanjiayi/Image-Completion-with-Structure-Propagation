#include "anchor.h"



double Anchor::computeES(){
    double result;

    return result;
}

double Anchor::computeEI(){
     double result;

     return result;
}

//vector<int> Anchor::neighbors{

//}

//vector<double> Anchor::E2{

//}

//void Anchor::getNeighbors(){

//}

void Anchor::computeE1(){
    this->E1=ks*computeES()+ki*computeEI();
}

//void Anchor::computeE2(){

//}
