#include "anchor.h"

Anchor::Anchor(int begin, int anchor, int end, PointType t) :
	begin_point(begin), anchor_point(anchor), end_point(end), type(t) , converged(false){
}


Anchor::~Anchor()
{
}

bool Anchor::if_converged()
{
	if (this->type != OUTER)
		return (this->converged);
	cout << "in if_converged() Error:encounter sample anchor" << endl;
	return false;
}

void Anchor::set_converge()
{
	if (this->type == OUTER) {
		cout << "in set_converge() Error:encounter sample anchor" << endl;
		return;
	}
	this->converged = true;
}


